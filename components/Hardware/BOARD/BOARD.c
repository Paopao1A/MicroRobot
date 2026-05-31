#include "BOARD.h"
#include "ADC_CC.h"
#include "BEEP_CC.h"
#include "Bluetooth_CC.h"
#include "IMU_CC.h"
#include "LIDAR_CC.h"
#include "LED.h"
#include "PWM_CC.h"
#include "SERVO_CC.h"
#include "TIMER_CC.h"
#include "UART_CC.h"
#include "WIFI_CC.h"

LED_Base_t *Esp_LED;
BEEP_Base_t *Esp_BEEP;
ADC_Base_t *Battery_ADC;
I2C_Base_t *Esp_I2C;
IMU_Base_t *Esp_IMU;
LIDAR_Base_t *Esp_LIDAR;
ENCODER_Base_t *Esp_ENCODER;
PWM_Base_t *Esp_PWM;
SERVO_Base_t *Esp_SERVO;
TIMER_Base_t *Esp_TIMER;
UART_Base_t *Esp_UART;
WIFI_Base_t *Esp_WIFI;

static ESPLED_Class_t s_led;
static ESPBEEP_Class_t s_beep;
static ESPBLUETOOTH_Class_t s_bluetooth;
static ESPADC_Class_t s_battery_adc;
static ESPI2C_Class_t s_i2c;
static ESPIMU_Class_t s_imu;
static ESPLIDAR_Class_t s_lidar;
static ESPENCODER_Class_t s_encoder;
static ESPPWM_Class_t s_pwm;
static ESPSERVO_Class_t s_servo;
static ESPTIMER_Class_t s_timer;
static ESPUART_Class_t s_uart;
static ESPWIFI_Class_t s_wifi;

//初始化板级硬件，所有硬件都在这里初始化，并将基类指针赋值给全局变量，供应用层调用
void BOARD_Init(void)
{
    ESPLED_Init(&s_led, "esp_led", LED_GPIO, 500);//初始化LED
    Esp_LED = &s_led.base;

    ESPBEEP_Init(&s_beep, "esp_beep", BEEP_GPIO);//初始化蜂鸣器
    Esp_BEEP = &s_beep.base;

    ESPBLUETOOTH_Init(&s_bluetooth, "esp_bluetooth",             //初始化蓝牙
                      "MicroRobot",
                      0xFFE0,
                      0xFFE1,
                      NULL,
                      NULL);
    Esp_Bluetooth = &s_bluetooth.base;

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

    ESPLIDAR_Init(&s_lidar, "ms200_lidar");//初始化雷达协议解析器
    Esp_LIDAR = &s_lidar.base;

    ESPENCODER_Init(&s_encoder, "esp_encoder");//初始化编码器
    Esp_ENCODER = &s_encoder.base;

    ESPPWM_Init(&s_pwm, "esp_pwm");//初始化PWM电机
    Esp_PWM = &s_pwm.base;

    ESPSERVO_Init(&s_servo, "esp_servo");//初始化舵机
    Esp_SERVO = &s_servo.base;

    ESPTIMER_Init(&s_timer, "esp_timer", TIMER_PERIOD_US);//初始化定时器
    Esp_TIMER = &s_timer.base;
    TIMER_Start();

    ESPUART_Init(&s_uart,       //初始化UART
                 "lidar_uart",
                 LIDAR_UART_NUM,
                 LIDAR_UART_TX_GPIO,
                 LIDAR_UART_RX_GPIO,
                 LIDAR_UART_BAUD_RATE,
                 LIDAR_UART_RX_BUFFER_SIZE,
                 LIDAR_UART_TX_BUFFER_SIZE);
    Esp_UART = &s_uart.base;

    ESPWIFI_Init(&s_wifi, "esp_wifi", WIFI_SSID, WIFI_PASSWORD, WIFI_MAXIMUM_RETRY);//初始化WIFI
    Esp_WIFI = &s_wifi.base;
    WIFI_Init();

}
