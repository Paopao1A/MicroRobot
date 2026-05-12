#include "BOARD.h"
#include "ADC_CC.h"
#include "BEEP_CC.h"
#include "IMU_CC.h"
#include "LED.h"
#include "PWM_CC.h"
#include "TIMER_CC.h"
#include "WIFI_CC.h"

LED_Base_t *Esp_LED;
BEEP_Base_t *Esp_BEEP;
ADC_Base_t *Battery_ADC;
I2C_Base_t *Esp_I2C;
IMU_Base_t *Esp_IMU;
ENCODER_Base_t *Esp_ENCODER;
PWM_Base_t *Esp_PWM;
TIMER_Base_t *Esp_TIMER;
WIFI_Base_t *Esp_WIFI;

static ESPLED_Class_t s_led;
static ESPBEEP_Class_t s_beep;
static ESPADC_Class_t s_battery_adc;
static ESPI2C_Class_t s_i2c;
static ESPIMU_Class_t s_imu;
static ESPENCODER_Class_t s_encoder;
static ESPPWM_Class_t s_pwm;
static ESPTIMER_Class_t s_timer;
static ESPWIFI_Class_t s_wifi;

void BOARD_Init(void)
{
    ESPLED_Init(&s_led, "esp_led", LED_GPIO, 500);//初始化LED
    Esp_LED = &s_led.base;

    ESPBEEP_Init(&s_beep, "esp_beep", BEEP_GPIO);//初始化蜂鸣器
    Esp_BEEP = &s_beep.base;

    ESPADC_Init(&s_battery_adc,                  //初始化电池电压ADC
                "battery_adc",
                ADC_UNIT_BATTERY,
                ADC_CHANNEL_BATTERY,
                ADC_ATTEN_BATTERY,
                ADC_BITWIDTH_BATTERY,
                BATTERY_VOLTAGE_RATIO);
    Battery_ADC = &s_battery_adc.base;

    ESPI2C_Init(&s_i2c,                          //初始化I2C    
                "esp_i2c",
                I2C_MASTER_NUM,
                I2C_MASTER_GPIO_SCL,
                I2C_MASTER_GPIO_SDA,
                I2C_MASTER_FREQ_HZ,
                I2C_MASTER_TIMEOUT_MS);
    Esp_I2C = &s_i2c.base;

    ESPIMU_Init(&s_imu, "icm42670p", ICM42670P_I2C_ADDR);//初始化IMU
    Esp_IMU = &s_imu.base;

    ESPENCODER_Init(&s_encoder, "esp_encoder");//初始化编码器
    Esp_ENCODER = &s_encoder.base;

    ESPPWM_Init(&s_pwm, "esp_pwm");//初始化PWM电机
    Esp_PWM = &s_pwm.base;

    ESPTIMER_Init(&s_timer, "esp_timer", TIMER_PERIOD_US);
    Esp_TIMER = &s_timer.base;
    TIMER_Start();

    ESPWIFI_Init(&s_wifi, "esp_wifi", WIFI_SSID, WIFI_PASSWORD, WIFI_MAXIMUM_RETRY);
    Esp_WIFI = &s_wifi.base;
    WIFI_Init();

}
