#include "LED_PC.h"

void LED_ON(LED_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->LED_ON != NULL);
    self->ops->LED_ON(self);
}

void LED_OFF(LED_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->LED_OFF != NULL);
    self->ops->LED_OFF(self);
}

void LED_FLASH(LED_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->LED_FLASH != NULL);
    self->ops->LED_FLASH(self);
}


