#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include <stdbool.h>
#include <stdint.h>

#include "Bluetooth_CC.h"
#include "Bluetooth_PC.h"

extern Bluetooth_Base_t *Esp_Bluetooth;

void Bluetooth_Init_ForAPP(Bluetooth_RxCallback_t rx_callback, void *rx_arg);
bool Bluetooth_Send(const char *data);

#endif
