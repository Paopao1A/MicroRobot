#ifndef __BOARD_H__
#define __BOARD_H__

#include "ADC.h"
#include "BEEP.h"
#include "Bluetooth.h"
#include "ENCODER.h"
#include "I2C.h"
#include "IMU.h"
#include "LED.h"
#include "PWM.h"
#include "TIMER.h"
#include "WIFI.h"

void BOARD_Init(void);

extern LED_Base_t *Esp_LED;
extern BEEP_Base_t *Esp_BEEP;
extern Bluetooth_Base_t *Esp_Bluetooth;
extern ADC_Base_t *Battery_ADC;
extern I2C_Base_t *Esp_I2C;
extern IMU_Base_t *Esp_IMU;
extern ENCODER_Base_t *Esp_ENCODER;
extern PWM_Base_t *Esp_PWM;
extern TIMER_Base_t *Esp_TIMER;
extern WIFI_Base_t *Esp_WIFI;

#endif
