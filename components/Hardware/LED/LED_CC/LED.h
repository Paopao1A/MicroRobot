#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>

#include "driver/gpio.h"
#include "LED_PC.h"

//定义LED子类
typedef struct
{
    LED_Base_t base;
    uint32_t gpio_num;
    uint16_t interval;
}ESPLED_Class_t;


#define LED_GPIO GPIO_NUM_45

void LED_Init(ESPLED_Class_t* self,char *name,uint32_t gpio_num,uint16_t interval);

#endif
