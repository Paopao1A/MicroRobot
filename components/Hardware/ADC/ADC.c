#include "ADC.h"
#include "BOARD.h"

volatile float Voltage_Data;//存储实际电池电压数据
volatile uint16_t Voltage_RawData;//存储电池电压原始数据

static ESPADC_Class_t s_legacy_battery_adc;//在板级初始化失效的时候，使用的备用ADC实例

static ADC_Base_t *ADC_Get_Default(void)//如果ADC未初始化，则初始化
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


void ADC_UpdateVoltageData(void)//更新电池电压数据
{
    ADC_UPDATE(ADC_Get_Default());
    Voltage_Data = ADC_GET_VOLTAGE(ADC_Get_Default());
    Voltage_RawData = ADC_GET_RAW_DATA(ADC_Get_Default());
}
