#ifndef __BLUETOOTH_PC_H__
#define __BLUETOOTH_PC_H__

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

typedef void (*Bluetooth_RxCallback_t)(const char *data, uint16_t len, void *arg);

typedef struct Bluetooth_Base Bluetooth_Base_t;

typedef struct
{
    void (*BLUETOOTH_START)(Bluetooth_Base_t *self);
    bool (*BLUETOOTH_SEND)(Bluetooth_Base_t *self, const char *data);
    void (*BLUETOOTH_SET_RX_CALLBACK)(Bluetooth_Base_t *self,
                                      Bluetooth_RxCallback_t rx_callback,
                                      void *rx_arg);
} Bluetooth_Ops_t;

struct Bluetooth_Base
{
    const char *name;
    const Bluetooth_Ops_t *ops;
};

void BLUETOOTH_START(Bluetooth_Base_t *self);
bool BLUETOOTH_SEND(Bluetooth_Base_t *self, const char *data);
void BLUETOOTH_SET_RX_CALLBACK(Bluetooth_Base_t *self,
                               Bluetooth_RxCallback_t rx_callback,
                               void *rx_arg);

#endif
