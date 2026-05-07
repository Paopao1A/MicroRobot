#ifndef __LED_H__
#define __LED_H__

#include "LED_CC.h"
#include "LED_PC.h"

void LED_On(LED_Base_t *self);
void LED_Off(LED_Base_t *self);
void LED_Flash(LED_Base_t *self);


#endif

