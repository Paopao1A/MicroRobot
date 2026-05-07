#include "LED.h"

void LED_Init(ESPLED_Class_t *self, const char *name, gpio_num_t gpio_num, uint16_t interval)
{
    ESPLED_Init(self, name, gpio_num, interval);
}

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