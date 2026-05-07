#ifndef __ENCODER_CC_H__
#define __ENCODER_CC_H__

#include "driver/pulse_cnt.h"
#include "ENCODER_PC.h"

#define ENCODER_GPIO_H1A          6
#define ENCODER_GPIO_H1B          7
#define ENCODER_GPIO_H2A          47
#define ENCODER_GPIO_H2B          48
#define ENCODER_GPIO_H3A          11
#define ENCODER_GPIO_H3B          12
#define ENCODER_GPIO_H4A          1
#define ENCODER_GPIO_H4B          2

#define ENCODER_PCNT_HIGH_LIMIT   1000
#define ENCODER_PCNT_LOW_LIMIT    -1000
#define ENCODER_NUM               4

#define ENCODER_COUNT_PER_REV     1040.0f // 1040为电机每转一圈编码器的计数值脉冲
#define ENCODER_SPEED_PERIOD_S    0.01f  // 速度计算周期0.01s

typedef struct
{
    ENCODER_Base_t base;
    pcnt_unit_handle_t units[ENCODER_NUM];
} ESPENCODER_Class_t;

void ESPENCODER_Init(ESPENCODER_Class_t *self, const char *name);

#endif
