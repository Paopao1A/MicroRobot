#include "WIFI_PC.h"

void WIFI_CONNECT(WIFI_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->WIFI_CONNECT != NULL);
    self->ops->WIFI_CONNECT(self);
}

bool WIFI_IS_CONNECTED(WIFI_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->WIFI_IS_CONNECTED != NULL);
    return self->ops->WIFI_IS_CONNECTED(self);
}
