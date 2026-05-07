#include "ADC_CC.h"

#include <stdbool.h>
#include "esp_adc/adc_cali_scheme.h"
#include "esp_err.h"
#include "Hardware_Common.h"

static void ESPADC_Update(ADC_Base_t *self);
static float ESPADC_GetVoltage(ADC_Base_t *self);
static uint16_t ESPADC_GetRawData(ADC_Base_t *self);

static const ADC_Ops_t ESP_ADC_Ops = {
    .ADC_Update = ESPADC_Update,
    .ADC_GetVoltage = ESPADC_GetVoltage,
    .ADC_GetRawData = ESPADC_GetRawData,
};

static void ESPADC_Cali_Deinit(ESPADC_Class_t *class)
{
    if (class->cali_handle != NULL)
    {
        ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(class->cali_handle));
        class->cali_handle = NULL;
    }
}

static bool ESPADC_Cali_Init(ESPADC_Class_t *class)
{
    ESPADC_Cali_Deinit(class);

    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = class->unit_id,
        .chan = class->channel,
        .atten = class->atten,
        .bitwidth = class->bitwidth,
    };

    return adc_cali_create_scheme_curve_fitting(&cali_config, &class->cali_handle) == ESP_OK;
}

static void ESPADC_Update(ADC_Base_t *self)
{
    ESPADC_Class_t *class = container_of(self, ESPADC_Class_t, base);
    int adc_raw = 0;
    int gpio_voltage_mv = 0;

    if (class->adc_handle == NULL)
    {
        return;
    }

    ESP_ERROR_CHECK(adc_oneshot_read(class->adc_handle, class->channel, &adc_raw));
    class->raw_data = (uint16_t)adc_raw;

    if (class->cali_handle != NULL &&
        adc_cali_raw_to_voltage(class->cali_handle, adc_raw, &gpio_voltage_mv) == ESP_OK)
    {
        class->voltage_data = gpio_voltage_mv / 1000.0f * class->voltage_ratio;
    }
}

static float ESPADC_GetVoltage(ADC_Base_t *self)
{
    ESPADC_Class_t *class = container_of(self, ESPADC_Class_t, base);
    return class->voltage_data;
}

static uint16_t ESPADC_GetRawData(ADC_Base_t *self)
{
    ESPADC_Class_t *class = container_of(self, ESPADC_Class_t, base);
    return class->raw_data;
}

void ESPADC_Init(ESPADC_Class_t *self,
                 const char *name,
                 adc_unit_t unit_id,
                 adc_channel_t channel,
                 adc_atten_t atten,
                 adc_bitwidth_t bitwidth,
                 float voltage_ratio)
{
    self->base.name = name;
    self->base.ops = &ESP_ADC_Ops;
    self->unit_id = unit_id;
    self->channel = channel;
    self->atten = atten;
    self->bitwidth = bitwidth;
    self->voltage_ratio = voltage_ratio;
    self->voltage_data = 0.0f;
    self->raw_data = 0;

    if (self->adc_handle != NULL)
    {
        ESP_ERROR_CHECK(adc_oneshot_del_unit(self->adc_handle));
        self->adc_handle = NULL;
    }

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = self->unit_id,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &self->adc_handle));

    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = self->bitwidth,
        .atten = self->atten,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(self->adc_handle, self->channel, &channel_config));

    ESPADC_Cali_Init(self);
}

