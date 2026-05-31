#ifndef __BOARD_H__
#define __BOARD_H__

#include "ADC.h"
#include "BEEP.h"
#include "Bluetooth.h"
#include "ENCODER.h"
#include "I2C.h"
#include "IMU.h"
#include "LIDAR.h"
#include "LED.h"
#include "PWM.h"
#include "SERVO.h"
#include "TIMER.h"
#include "UART.h"
#include "WIFI.h"

void BOARD_Init(void);

extern LED_Base_t *Esp_LED;
extern BEEP_Base_t *Esp_BEEP;
extern Bluetooth_Base_t *Esp_Bluetooth;
extern ADC_Base_t *Battery_ADC;
extern I2C_Base_t *Esp_I2C;
extern IMU_Base_t *Esp_IMU;
extern LIDAR_Base_t *Esp_LIDAR;
extern ENCODER_Base_t *Esp_ENCODER;
extern PWM_Base_t *Esp_PWM;
extern SERVO_Base_t *Esp_SERVO;
extern TIMER_Base_t *Esp_TIMER;
extern UART_Base_t *Esp_UART;
extern WIFI_Base_t *Esp_WIFI;

#endif
