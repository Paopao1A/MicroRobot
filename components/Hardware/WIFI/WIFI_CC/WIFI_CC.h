#ifndef __WIFI_CC_H__
#define __WIFI_CC_H__

#include <stdbool.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_netif_types.h"
#include "WIFI_PC.h"

#define WIFI_SSID           "tttttttt"  //WIFI名称
#define WIFI_PASSWORD       "8888tttt"  //WIFI密码
#define WIFI_MAXIMUM_RETRY  5           //最大重试次数

typedef struct
{
    WIFI_Base_t base;
    EventGroupHandle_t event_group;
    esp_netif_t *netif;
    const char *ssid;
    const char *password;
    int maximum_retry;
    int reconnect_count;
    bool is_initialized;
    bool is_connected;
} ESPWIFI_Class_t;

void ESPWIFI_Init(ESPWIFI_Class_t *self,
                  const char *name,
                  const char *ssid,
                  const char *password,
                  int maximum_retry);

#endif
