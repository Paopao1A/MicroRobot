#ifndef __ADC_CC_H__
#define __ADC_CC_H__

#include <stdint.h>

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "ADC_PC.h"

#define ADC_ATTEN_BATTERY       ADC_ATTEN_DB_11
#define ADC_CHANNEL_BATTERY     ADC_CHANNEL_2
#define ADC_UNIT_BATTERY        ADC_UNIT_1
#define ADC_BITWIDTH_BATTERY    ADC_BITWIDTH_DEFAULT
#define BATTERY_VOLTAGE_RATIO   4.03f

typedef struct
{
    ADC_Base_t base;
    adc_unit_t unit_id;
    adc_channel_t channel;
    adc_atten_t atten;
    adc_bitwidth_t bitwidth;
    float voltage_ratio;
    adc_oneshot_unit_handle_t adc_handle;
    adc_cali_handle_t cali_handle;
    float voltage_data;
    uint16_t raw_data;
} ESPADC_Class_t;

void ESPADC_Init(ESPADC_Class_t *self,
                 const char *name,
                 adc_unit_t unit_id,
                 adc_channel_t channel,
                 adc_atten_t atten,
                 adc_bitwidth_t bitwidth,
                 float voltage_ratio);

#endif

