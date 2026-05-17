#include "Bluetooth_CC.h"

#include <string.h>

#include "Hardware_Common.h"
#include "esp_log.h"

#if CONFIG_BT_NIMBLE_ENABLED
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#endif

#define BLUETOOTH_PACKET_MAX_LEN 96

static const char *TAG = "Bluetooth";

static void ESPBLUETOOTH_Start(Bluetooth_Base_t *self);
static bool ESPBLUETOOTH_Send(Bluetooth_Base_t *self, const char *data);
static void ESPBLUETOOTH_SetRxCallback(Bluetooth_Base_t *self,
                                       Bluetooth_RxCallback_t rx_callback,
                                       void *rx_arg);

static const Bluetooth_Ops_t ESP_Bluetooth_Ops = {
    .BLUETOOTH_START = ESPBLUETOOTH_Start,
    .BLUETOOTH_SEND = ESPBLUETOOTH_Send,
    .BLUETOOTH_SET_RX_CALLBACK = ESPBLUETOOTH_SetRxCallback,
};

#if CONFIG_BT_NIMBLE_ENABLED
static ESPBLUETOOTH_Class_t *s_active_bluetooth = NULL;
static uint16_t s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
static uint16_t s_char_handle = 0;
static uint8_t s_own_addr_type = BLE_OWN_ADDR_PUBLIC;
static ble_uuid16_t s_service_uuid = BLE_UUID16_INIT(0xFFE0);
static ble_uuid16_t s_char_uuid = BLE_UUID16_INIT(0xFFE1);

//GATT访问回调函数，也就是接收蓝牙数据
static int ESPBLUETOOTH_GattAccess(uint16_t conn_handle,
                                   uint16_t attr_handle,
                                   struct ble_gatt_access_ctxt *ctxt,
                                   void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR)
    {
        char buffer[BLUETOOTH_PACKET_MAX_LEN];
        uint16_t len = OS_MBUF_PKTLEN(ctxt->om);
        if (len >= sizeof(buffer))
        {
            len = sizeof(buffer) - 1;
        }

        int ret = ble_hs_mbuf_to_flat(ctxt->om, buffer, len, NULL);
        if (ret != 0)
        {
            return BLE_ATT_ERR_UNLIKELY;
        }

        buffer[len] = '\0';
        if ((s_active_bluetooth != NULL) &&
            (s_active_bluetooth->rx_callback != NULL))
        {//调用注册的接收回调函数
            s_active_bluetooth->rx_callback(buffer, len, s_active_bluetooth->rx_arg);
        }

        return 0;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

static const struct ble_gatt_svc_def s_gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &s_service_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = &s_char_uuid.u,
                .access_cb = ESPBLUETOOTH_GattAccess,
                .val_handle = &s_char_handle,
                .flags = BLE_GATT_CHR_F_WRITE |
                         BLE_GATT_CHR_F_WRITE_NO_RSP |
                         BLE_GATT_CHR_F_NOTIFY,
            },
            {0},
        },
    },
    {0},
};

static int ESPBLUETOOTH_GapEvent(struct ble_gap_event *event, void *arg);

static void ESPBLUETOOTH_Advertise(void)//广告蓝牙服务
{
    if (s_active_bluetooth == NULL)
    {
        return;
    }

    struct ble_hs_adv_fields fields = {0};
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.name = (const uint8_t *)s_active_bluetooth->device_name;
    fields.name_len = strlen(s_active_bluetooth->device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    struct ble_gap_adv_params adv_params = {0};
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    ble_gap_adv_start(s_own_addr_type,
                      NULL,
                      BLE_HS_FOREVER,
                      &adv_params,
                      ESPBLUETOOTH_GapEvent,
                      NULL);
}

static int ESPBLUETOOTH_GapEvent(struct ble_gap_event *event, void *arg)
{
    (void)arg;

    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0)
        {
            s_conn_handle = event->connect.conn_handle;

            struct ble_gap_upd_params params = {0};
            params.itvl_min = 6;
            params.itvl_max = 12;
            params.latency = 0;
            params.supervision_timeout = 400;
            ble_gap_update_params(s_conn_handle, &params);
        }
        else
        {
            ESPBLUETOOTH_Advertise();
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
        ESPBLUETOOTH_Advertise();
        break;
    case BLE_GAP_EVENT_SUBSCRIBE:
        s_conn_handle = event->subscribe.conn_handle;
        break;
    default:
        break;
    }

    return 0;
}

static void ESPBLUETOOTH_OnSync(void)
{
    int ret = ble_hs_id_infer_auto(0, &s_own_addr_type);
    if (ret != 0)
    {
        ESP_LOGE(TAG, "ble_hs_id_infer_auto failed: %d", ret);
        return;
    }

    ESPBLUETOOTH_Advertise();
}

static void ESPBLUETOOTH_HostTask(void *param)//NimBLE主机任务，这一步完成蓝牙的连接
{
    (void)param;

    nimble_port_run();
    nimble_port_freertos_deinit();
}
#endif

static void ESPBLUETOOTH_Start(Bluetooth_Base_t *self)
{
    ESPBLUETOOTH_Class_t *class = container_of(self, ESPBLUETOOTH_Class_t, base);

    if (class->is_initialized)
    {
        return;
    }

#if CONFIG_BT_NIMBLE_ENABLED
    s_active_bluetooth = class;
    s_service_uuid.value = class->service_uuid;
    s_char_uuid.value = class->char_uuid;

    ESP_ERROR_CHECK(nimble_port_init());

    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_svc_gap_device_name_set(class->device_name);

    ble_hs_cfg.sync_cb = ESPBLUETOOTH_OnSync;
    ble_hs_cfg.gatts_register_cb = NULL;
    ble_hs_cfg.store_status_cb = NULL;

    ESP_ERROR_CHECK(ble_gatts_count_cfg(s_gatt_svcs));
    ESP_ERROR_CHECK(ble_gatts_add_svcs(s_gatt_svcs));

    nimble_port_freertos_init(ESPBLUETOOTH_HostTask);
    class->is_initialized = true;
    ESP_LOGI(TAG, "BLE serial service started");
#else
    ESP_LOGW(TAG, "NimBLE is disabled");
#endif
}

//发送数据到蓝牙
static bool ESPBLUETOOTH_Send(Bluetooth_Base_t *self, const char *data)
{
    (void)self;

#if CONFIG_BT_NIMBLE_ENABLED
    if ((data == NULL) ||
        (s_conn_handle == BLE_HS_CONN_HANDLE_NONE) ||
        (s_char_handle == 0))
    {
        return false;
    }

    struct os_mbuf *om = ble_hs_mbuf_from_flat(data, strlen(data));
    if (om == NULL)
    {
        return false;
    }

    return ble_gatts_notify_custom(s_conn_handle, s_char_handle, om) == 0;
#else
    (void)data;
    return false;
#endif
}

static void ESPBLUETOOTH_SetRxCallback(Bluetooth_Base_t *self,
                                       Bluetooth_RxCallback_t rx_callback,
                                       void *rx_arg)
{
    ESPBLUETOOTH_Class_t *class = container_of(self, ESPBLUETOOTH_Class_t, base);

    class->rx_callback = rx_callback;
    class->rx_arg = rx_arg;
}

void ESPBLUETOOTH_Init(ESPBLUETOOTH_Class_t *self,
                       const char *name,
                       const char *device_name,
                       uint16_t service_uuid,
                       uint16_t char_uuid,
                       Bluetooth_RxCallback_t rx_callback,
                       void *rx_arg)
{
    self->base.name = name;
    self->base.ops = &ESP_Bluetooth_Ops;
    self->device_name = device_name;
    self->service_uuid = service_uuid;
    self->char_uuid = char_uuid;
    self->rx_callback = rx_callback;
    self->rx_arg = rx_arg;
    self->is_initialized = false;
}
