#ifndef __BEEP_CC_H__
#define __BEEP_CC_H__

#include <stdint.h>

#include "driver/gpio.h"
#include "BEEP_PC.h"

#define BEEP_GPIO GPIO_NUM_46

typedef struct
{
    BEEP_Base_t base;
    uint32_t gpio_num;
} ESPBEEP_Class_t;

void ESPBEEP_Init(ESPBEEP_Class_t *self, const char *name, uint32_t gpio_num);

#endif

