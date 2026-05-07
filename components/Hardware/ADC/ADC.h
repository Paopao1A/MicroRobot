#ifndef __ADC_H__
#define __ADC_H__

#include "ADC_CC.h"
#include "ADC_PC.h"

extern volatile float Voltage_Data;
extern volatile uint16_t Voltage_RawData;

void ADC_UpdateVoltageData(void);

#endif

