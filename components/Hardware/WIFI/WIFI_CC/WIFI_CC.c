#include "WIFI_CC.h"

#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "Hardware_Common.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "WIFI";

static void ESPWIFI_Connect(WIFI_Base_t *self);
static bool ESPWIFI_IsConnected(WIFI_Base_t *self);

static const WIFI_Ops_t ESP_WIFI_Ops = {
    .WIFI_CONNECT = ESPWIFI_Connect,
    .WIFI_IS_CONNECTED = ESPWIFI_IsConnected,
};

static void ESPWIFI_EventHandler(void *arg,
                                 esp_event_base_t event_base,
                                 int32_t event_id,
                                 void *event_data)
{
    ESPWIFI_Class_t *class = (ESPWIFI_Class_t *)arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        class->is_connected = false;

        if (class->reconnect_count < class->maximum_retry)
        {
            esp_wifi_connect();
            class->reconnect_count++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(class->event_group, WIFI_FAIL_BIT);
        }

        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));

        class->reconnect_count = 0;
        class->is_connected = true;
        xEventGroupSetBits(class->event_group, WIFI_CONNECTED_BIT);
    }
}

static void ESPWIFI_NVSInit(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
}

static void ESPWIFI_Connect(WIFI_Base_t *self)
{
    ESPWIFI_Class_t *class = container_of(self, ESPWIFI_Class_t, base);

    if (class->is_connected)
    {
        return;
    }

    if (!class->is_initialized)
    {
        ESPWIFI_NVSInit();
        ESP_ERROR_CHECK(esp_netif_init());

        esp_err_t ret = esp_event_loop_create_default();
        if (ret != ESP_ERR_INVALID_STATE)
        {
            ESP_ERROR_CHECK(ret);
        }

        esp_netif_create_default_wifi_sta();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                            ESP_EVENT_ANY_ID,
                                                            &ESPWIFI_EventHandler,
                                                            class,
                                                            NULL));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                            IP_EVENT_STA_GOT_IP,
                                                            &ESPWIFI_EventHandler,
                                                            class,
                                                            NULL));

        wifi_config_t wifi_config = {0};
        snprintf((char *)wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), "%s", class->ssid);
        snprintf((char *)wifi_config.sta.password, sizeof(wifi_config.sta.password), "%s", class->password);
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());

        class->is_initialized = true;
    }
    else
    {
        ESP_ERROR_CHECK(esp_wifi_connect());
    }

    ESP_LOGI(TAG, "wifi_init_sta finished");

    EventBits_t bits = xEventGroupWaitBits(class->event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s", class->ssid);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "failed to connect to SSID:%s", class->ssid);
    }
    else
    {
        ESP_LOGE(TAG, "unexpected event");
    }
}

static bool ESPWIFI_IsConnected(WIFI_Base_t *self)
{
    ESPWIFI_Class_t *class = container_of(self, ESPWIFI_Class_t, base);
    return class->is_connected;
}

void ESPWIFI_Init(ESPWIFI_Class_t *self,
                  const char *name,
                  const char *ssid,
                  const char *password,
                  int maximum_retry)
{
    self->base.name = name;
    self->base.ops = &ESP_WIFI_Ops;
    self->event_group = xEventGroupCreate();
    self->ssid = ssid;
    self->password = password;
    self->maximum_retry = maximum_retry;
    self->reconnect_count = 0;
    self->is_initialized = false;
    self->is_connected = false;
}
