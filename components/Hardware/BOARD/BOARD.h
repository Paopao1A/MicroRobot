#ifndef __BOARD_H__
#define __BOARD_H__

#include "ADC_PC.h"
#include "BEEP_PC.h"
#include "ENCODER_PC.h"
#include "I2C_PC.h"
#include "LED_PC.h"

void BOARD_Init(void);

extern LED_Base_t *Esp_LED;
extern BEEP_Base_t *Esp_BEEP;
extern ADC_Base_t *Battery_ADC;
extern ENCODER_Base_t *Chassis_Encoder;
extern I2C_Base_t *Esp_I2C;

#endif

