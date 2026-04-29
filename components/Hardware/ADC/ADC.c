#include "ADC.h"
#include <stdbool.h>
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"



volatile float Voltage_Data;
volatile uint16_t Voltage_RawData;

static adc_oneshot_unit_handle_t s_adc_handle;
static adc_cali_handle_t s_adc_cali_handle;

static void ADC_Cali_Deinit(void)
{
    if (s_adc_cali_handle != NULL) {
        ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(s_adc_cali_handle));
        s_adc_cali_handle = NULL;
    }
}

static bool ADC_Cali_Init(void)
{
    ADC_Cali_Deinit();

    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_BATTERY,
        .chan = ADC_CHANNEL_BATTERY,
        .atten = ADC_ATTEN_BATTERY,
        .bitwidth = ADC_BITWIDTH_BATTERY,
    };

    return adc_cali_create_scheme_curve_fitting(&cali_config, &s_adc_cali_handle) == ESP_OK;
}

void ADC_Init(void)
{
    if (s_adc_handle != NULL) {
        ESP_ERROR_CHECK(adc_oneshot_del_unit(s_adc_handle));
        s_adc_handle = NULL;
    }

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_BATTERY,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &s_adc_handle));

    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = ADC_BITWIDTH_BATTERY,
        .atten = ADC_ATTEN_BATTERY,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(s_adc_handle, ADC_CHANNEL_BATTERY, &channel_config));

    ADC_Cali_Init();
}

void ADC_UpdateVoltageData(void)
{
    int adc_raw = 0;
    int gpio_voltage_mv = 0;

    if (s_adc_handle == NULL) 
    {
        return;
    }

    ESP_ERROR_CHECK(adc_oneshot_read(s_adc_handle, ADC_CHANNEL_BATTERY, &adc_raw));
    Voltage_RawData = (uint16_t)adc_raw;

    if (s_adc_cali_handle != NULL &&
        adc_cali_raw_to_voltage(s_adc_cali_handle, adc_raw, &gpio_voltage_mv) == ESP_OK)
    {
        Voltage_Data = gpio_voltage_mv / 1000.0f * BATTERY_VOLTAGE_RATIO;
    } 
}
