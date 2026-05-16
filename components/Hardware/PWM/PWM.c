#include "PWM.h"
#include "BOARD.h"

static ESPPWM_Class_t s_legacy_pwm;

static PWM_Base_t *PWM_Get_Default(void)
{
    if (Esp_PWM == NULL)
    {
        ESPPWM_Init(&s_legacy_pwm, "esp_pwm");
        Esp_PWM = &s_legacy_pwm.base;
    }

    return Esp_PWM;
}

void PWM_Set_Duty(pwm_id_t pwm_id, int duty_cycle)//设置不同电机占空比，范围-200~200
{
    PWM_SET(PWM_Get_Default(), pwm_id, duty_cycle);
}

void PWM_Stop_Motor(pwm_id_t pwm_id, bool brake)//停止电机，brake=true代表启动刹车停止，false就是正常滑动停止
{
    PWM_STOP(PWM_Get_Default(), pwm_id, brake);
}
