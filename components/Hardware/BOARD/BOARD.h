#ifndef __BOARD_H__
#define __BOARD_H__

#include "ADC.h"
#include "BEEP.h"
#include "ENCODER.h"
#include "I2C.h"
#include "LED.h"

void BOARD_Init(void);

extern LED_Base_t *Esp_LED;
extern BEEP_Base_t *Esp_BEEP;
extern ADC_Base_t *Battery_ADC;
extern ENCODER_Base_t *Chassis_Encoder;
extern I2C_Base_t *Esp_I2C;

#endif

