#include "BOARD.h"
#include "ADC_CC.h"
#include "BEEP_CC.h"
#include "LED.h"

LED_Base_t *Esp_LED;
BEEP_Base_t *Esp_BEEP;
ADC_Base_t *Battery_ADC;
ENCODER_Base_t *Chassis_Encoder;
I2C_Base_t *Esp_I2C;

static ESPLED_Class_t s_led;
static ESPBEEP_Class_t s_beep;
static ESPADC_Class_t s_battery_adc;

void BOARD_Init(void)
{
    LED_Init(&s_led, "esp_led", LED_GPIO, 500);
    Esp_LED = &s_led.base;

    ESPBEEP_Init(&s_beep, "esp_beep", BEEP_GPIO);
    Esp_BEEP = &s_beep.base;

    ESPADC_Init(&s_battery_adc,
                "battery_adc",
                ADC_UNIT_BATTERY,
                ADC_CHANNEL_BATTERY,
                ADC_ATTEN_BATTERY,
                ADC_BITWIDTH_BATTERY,
                BATTERY_VOLTAGE_RATIO);
    Battery_ADC = &s_battery_adc.base;

    Chassis_Encoder = NULL;
    Esp_I2C = NULL;
}
