#ifndef __SERVO_CC_H__
#define __SERVO_CC_H__

#include <stdint.h>

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "SERVO_PC.h"

#define SERVO_GPIO_PAN                 GPIO_NUM_8   //底部舵机GPIO引脚
#define SERVO_GPIO_TILT                GPIO_NUM_21 //顶部舵机GPIO引脚

#define SERVO_NUM                      2 //舵机数量
#define SERVO_FREQ_HZ                  50 //舵机频率，单位Hz
#define SERVO_TIMER_RESOLUTION         LEDC_TIMER_14_BIT //舵机占空比分辨率
#define SERVO_TIMER                    LEDC_TIMER_0 //舵机LEDC定时器
#define SERVO_MODE                     LEDC_LOW_SPEED_MODE //舵机LEDC速度模式
#define SERVO_PULSE_MIN_US             500 //舵机最小脉冲宽度，单位微秒
#define SERVO_PULSE_MAX_US             2500 //舵机最大脉冲宽度，单位微秒
#define SERVO_ANGLE_MIN_DEG            0.0f //舵机最小角度，单位度数
#define SERVO_ANGLE_MAX_DEG            180.0f //舵机最大角度，单位度数
#define SERVO_ANGLE_CENTER_DEG         90.0f //舵机中心角度，单位度数
#define SERVO_PAN_DEFAULT_DEG          90.0f //底部舵机默认角度，单位度数
#define SERVO_PAN_MIN_DEG              0.0f //底部舵机最小角度，单位度数
#define SERVO_PAN_MAX_DEG              180.0f //底部舵机最大角度，单位度数
#define SERVO_TILT_DEFAULT_DEG         30.0f //顶部舵机默认角度，单位度数
#define SERVO_TILT_MIN_DEG             0.0f //顶部舵机最小角度，单位度数
#define SERVO_TILT_MAX_DEG             110.0f //顶部舵机最大角度，单位度数

typedef struct
{
    gpio_num_t gpio_num;
    ledc_channel_t channel;
} ESPSERVO_Channel_t;

typedef struct
{
    SERVO_Base_t base;
    ESPSERVO_Channel_t servos[SERVO_NUM];
} ESPSERVO_Class_t;

void ESPSERVO_Init(ESPSERVO_Class_t *self, const char *name);

#endif
