#ifndef __PWM_CC_H__
#define __PWM_CC_H__

#include <stdbool.h>

#include "driver/mcpwm_prelude.h"
#include "PWM_PC.h"

#define PWM_GPIO_M1A                    4  //电机1A引脚
#define PWM_GPIO_M1B                    5  //电机1B引脚，A/B引脚分别控制电机正反转
#define PWM_GPIO_M2A                    15
#define PWM_GPIO_M2B                    16
#define PWM_GPIO_M3A                    9
#define PWM_GPIO_M3B                    10
#define PWM_GPIO_M4A                    13
#define PWM_GPIO_M4B                    14

#define PWM_MOTOR_TIMER_RESOLUTION_HZ   10000000    //定时器频率10MHZ
#define PWM_MOTOR_FREQ_HZ               25000       //PWM频率25KHz
#define PWM_MOTOR_DUTY_TICK_MAX         (PWM_MOTOR_TIMER_RESOLUTION_HZ / PWM_MOTOR_FREQ_HZ)//相当于ARR值
#define PWM_MOTOR_DEAD_ZONE             200       //输出直接加200，防止电机无法启动
#define PWM_MOTOR_MAX_VALUE             (PWM_MOTOR_DUTY_TICK_MAX - PWM_MOTOR_DEAD_ZONE)//相当于CRR值

#define PWM_MOTOR_TIMER_GROUP_ID_M1     0
#define PWM_MOTOR_TIMER_GROUP_ID_M2     0
#define PWM_MOTOR_TIMER_GROUP_ID_M3     0
#define PWM_MOTOR_TIMER_GROUP_ID_M4     1

#define PWM_MOTOR_NUM                   4

typedef struct
{
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t oper;
    mcpwm_cmpr_handle_t comparator_a;//双输入驱动，所以需要两个比较器，分别对应A/B引脚
    mcpwm_cmpr_handle_t comparator_b;
    mcpwm_gen_handle_t generator_a;
    mcpwm_gen_handle_t generator_b;
} ESPPWM_Motor_t;

typedef struct
{
    PWM_Base_t base;
    ESPPWM_Motor_t motors[PWM_MOTOR_NUM];
} ESPPWM_Class_t;

void ESPPWM_Init(ESPPWM_Class_t *self, const char *name);

#endif
