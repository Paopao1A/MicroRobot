#include "LED_PC.h"

void LED_On(LED_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->LED_On != NULL);
    self->ops->LED_On(self);
}

void LED_Off(LED_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->LED_Off != NULL);
    self->ops->LED_Off(self);
}

void LED_Flash(LED_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->LED_Flash != NULL);
    self->ops->LED_Flash(self);
}
