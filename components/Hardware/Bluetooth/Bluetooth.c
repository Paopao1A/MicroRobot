#include "Bluetooth.h"
#include "stdio.h"

#define BLUETOOTH_DEVICE_NAME "MicroRobot"
#define BLUETOOTH_SERVICE_UUID 0xFFE0       
#define BLUETOOTH_CHAR_UUID    0xFFE1

static ESPBLUETOOTH_Class_t s_legacy_bluetooth;
Bluetooth_Base_t *Esp_Bluetooth = NULL;

static Bluetooth_Base_t *Bluetooth_Get_Default(Bluetooth_RxCallback_t rx_callback, void *rx_arg)
{
    if (Esp_Bluetooth == NULL)
    {
        ESPBLUETOOTH_Init(&s_legacy_bluetooth,
                          "esp_bluetooth",
                          BLUETOOTH_DEVICE_NAME,
                          BLUETOOTH_SERVICE_UUID,
                          BLUETOOTH_CHAR_UUID,
                          rx_callback,
                          rx_arg);
        Esp_Bluetooth = &s_legacy_bluetooth.base;
    }

    return Esp_Bluetooth;
}

void Bluetooth_Init_ForAPP(Bluetooth_RxCallback_t rx_callback, void *rx_arg)
{
    Bluetooth_Base_t *bluetooth = Bluetooth_Get_Default(rx_callback, rx_arg);

    BLUETOOTH_SET_RX_CALLBACK(bluetooth, rx_callback, rx_arg);//注册接收回调函数
    BLUETOOTH_START(bluetooth);//正式启动蓝牙
}

bool Bluetooth_Send(const char *data)
{
    if (Esp_Bluetooth == NULL)
    {
        return false;
    }

    return BLUETOOTH_SEND(Esp_Bluetooth, data);
}
