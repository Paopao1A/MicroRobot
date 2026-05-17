#include "Bluetooth_PC.h"
#include "stdio.h"

void BLUETOOTH_START(Bluetooth_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->BLUETOOTH_START != NULL);
    self->ops->BLUETOOTH_START(self);
}

bool BLUETOOTH_SEND(Bluetooth_Base_t *self, const char *data)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->BLUETOOTH_SEND != NULL);
    return self->ops->BLUETOOTH_SEND(self, data);
}

void BLUETOOTH_SET_RX_CALLBACK(Bluetooth_Base_t *self,
                               Bluetooth_RxCallback_t rx_callback,
                               void *rx_arg)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->BLUETOOTH_SET_RX_CALLBACK != NULL);
    self->ops->BLUETOOTH_SET_RX_CALLBACK(self, rx_callback, rx_arg);
}
