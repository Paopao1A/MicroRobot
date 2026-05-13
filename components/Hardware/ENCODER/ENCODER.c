#include "ENCODER.h"
#include "BOARD.h"

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static ESPENCODER_Class_t s_legacy_encoder;

static ENCODER_Base_t *ENCODER_Get_Default(void)
{
    if (Esp_ENCODER == NULL)
    {
        ESPENCODER_Init(&s_legacy_encoder, "esp_encoder");
        Esp_ENCODER = &s_legacy_encoder.base;
    }

    return Esp_ENCODER;
}


int Encoder_Get_Count(encoder_id_t encoder_id)// 获取电机转数
{
    return ENCODER_GET_COUNT(ENCODER_Get_Default(), encoder_id);
}

float Encoder_Get_Speed(encoder_id_t encoder_id)// 获取电机转速，单位转/min
{
    static int last_count[ENCODER_NUM] = {0};
    int index = (int)encoder_id - 1;

    if (index < 0 || index >= ENCODER_NUM)
    {
        return 0.0f;
    }

    int current_count = Encoder_Get_Count(encoder_id);
    int error_count = current_count - last_count[index];
    float speed = (float)error_count /(ENCODER_COUNT_PER_REV * ENCODER_SPEED_PERIOD_S) * 60.0f; // 转换为转/分钟

    last_count[index] = current_count;
    return speed;
}
