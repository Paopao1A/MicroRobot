#ifndef __LED_H__
#define __LED_H__

#include "stdio.h"

#define LED_GPIO GPIO_NUM_45

void LED_Init(void);
void LED_On(void);
void LED_Off(void);
void LED_Flash(uint16_t interval);


#endif
