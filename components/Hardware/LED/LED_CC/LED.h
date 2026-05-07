#ifndef __LED_H__
#define __LED_H__

#include "LED_CC.h"

void LED_Init(ESPLED_Class_t *self, const char *name, gpio_num_t gpio_num, uint16_t interval);

#endif

