#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

#define ADC_ATTEN_BATTERY               ADC_ATTEN_DB_11
#define ADC_CHANNEL_BATTERY             ADC_CHANNEL_2
#define ADC_UNIT_BATTERY                ADC_UNIT_1
#define ADC_BITWIDTH_BATTERY            ADC_BITWIDTH_DEFAULT
#define BATTERY_VOLTAGE_RATIO           4.03f

extern volatile float Voltage_Data; // 电压数据
extern volatile uint16_t Voltage_RawData;  // 原始电压数据

void ADC_Init(void);
void ADC_UpdateVoltageData(void);

#endif
