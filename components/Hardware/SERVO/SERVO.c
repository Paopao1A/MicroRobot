#include "SERVO.h"

#include "BOARD.h"

static ESPSERVO_Class_t s_legacy_servo;

static SERVO_Base_t *SERVO_Get_Default(void)
{
    if (Esp_SERVO == NULL)
    {
        ESPSERVO_Init(&s_legacy_servo, "esp_servo");
        Esp_SERVO = &s_legacy_servo.base;
    }

    return Esp_SERVO;
}

void SERVO_Set_Angle(servo_id_t servo_id, float angle_deg)//设置舵机角度
{
    SERVO_SET_ANGLE(SERVO_Get_Default(), servo_id, angle_deg);
}

void SERVO_Set_PulseUs(servo_id_t servo_id, uint32_t pulse_us)//设置舵机脉冲宽度
{
    SERVO_SET_PULSE_US(SERVO_Get_Default(), servo_id, pulse_us);
}
