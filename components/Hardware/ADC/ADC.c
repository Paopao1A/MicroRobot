#include "ADC.h"
#include "BOARD.h"

volatile float Voltage_Data;
volatile uint16_t Voltage_RawData;

static ESPADC_Class_t s_legacy_battery_adc;

static ADC_Base_t *ADC_Get_Default(void)
{
    if (Battery_ADC == NULL)
    {
        ESPADC_Init(&s_legacy_battery_adc,
                    "battery_adc",
                    ADC_UNIT_BATTERY,
                    ADC_CHANNEL_BATTERY,
                    ADC_ATTEN_BATTERY,
                    ADC_BITWIDTH_BATTERY,
                    BATTERY_VOLTAGE_RATIO);
        Battery_ADC = &s_legacy_battery_adc.base;
    }

    return Battery_ADC;
}

void ADC_Init(void)
{
    ESPADC_Init(&s_legacy_battery_adc,
                "battery_adc",
                ADC_UNIT_BATTERY,
                ADC_CHANNEL_BATTERY,
                ADC_ATTEN_BATTERY,
                ADC_BITWIDTH_BATTERY,
                BATTERY_VOLTAGE_RATIO);
    Battery_ADC = &s_legacy_battery_adc.base;
}

void ADC_UpdateVoltageData(void)
{
    ADC_Base_t *adc = ADC_Get_Default();

    ADC_Update(adc);
    Voltage_Data = ADC_GetVoltage(adc);
    Voltage_RawData = ADC_GetRawData(adc);
}

