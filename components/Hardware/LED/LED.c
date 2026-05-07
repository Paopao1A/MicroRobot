#include "LED.h"


void LED_On(LED_Base_t *self)
{
    LED_ON(self);
}

void LED_Off(LED_Base_t *self)
{
    LED_OFF(self);
}

void LED_Flash(LED_Base_t *self)
{
    LED_FLASH(self);
}