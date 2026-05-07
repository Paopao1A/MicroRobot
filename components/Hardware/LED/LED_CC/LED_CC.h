#ifndef __LED_CC_H__
#define __LED_CC_H__

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "LED_PC.h"

#define LED_GPIO GPIO_NUM_45

typedef struct
{
    LED_Base_t base;
    gpio_num_t gpio_num;
    uint16_t interval;
    uint16_t flash_count;
    bool flash_state;
} ESPLED_Class_t;

void ESPLED_Init(ESPLED_Class_t *self, const char *name, gpio_num_t gpio_num, uint16_t interval);

#endif

