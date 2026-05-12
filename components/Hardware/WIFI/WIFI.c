#include "WIFI.h"
#include "BOARD.h"

static ESPWIFI_Class_t s_legacy_wifi;

static WIFI_Base_t *WIFI_Get_Default(void)
{
    if (Esp_WIFI == NULL)
    {
        ESPWIFI_Init(&s_legacy_wifi, "esp_wifi", WIFI_SSID, WIFI_PASSWORD, WIFI_MAXIMUM_RETRY);
        Esp_WIFI = &s_legacy_wifi.base;
    }

    return Esp_WIFI;
}

void WIFI_Init(void)
{
    WIFI_CONNECT(WIFI_Get_Default());
}

bool WIFI_IsConnected(void)
{
    return WIFI_IS_CONNECTED(WIFI_Get_Default());
}
