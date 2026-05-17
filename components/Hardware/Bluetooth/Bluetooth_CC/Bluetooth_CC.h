#ifndef __BLUETOOTH_CC_H__
#define __BLUETOOTH_CC_H__

#include <stdbool.h>
#include <stdint.h>

#include "Bluetooth_PC.h"

typedef struct
{
    Bluetooth_Base_t base;
    const char *device_name;
    uint16_t service_uuid;
    uint16_t char_uuid;
    Bluetooth_RxCallback_t rx_callback;
    void *rx_arg;
    bool is_initialized;
} ESPBLUETOOTH_Class_t;

void ESPBLUETOOTH_Init(ESPBLUETOOTH_Class_t *self,
                       const char *name,
                       const char *device_name,
                       uint16_t service_uuid,
                       uint16_t char_uuid,
                       Bluetooth_RxCallback_t rx_callback,
                       void *rx_arg);

#endif
