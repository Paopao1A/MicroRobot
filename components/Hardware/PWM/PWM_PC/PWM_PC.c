#include "PWM_PC.h"

void PWM_SET(PWM_Base_t *self, pwm_id_t pwm_id, int duty_cycle)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->PWM_SET != NULL);
    self->ops->PWM_SET(self, pwm_id, duty_cycle);
}

void PWM_STOP(PWM_Base_t *self, pwm_id_t pwm_id, bool brake)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->PWM_STOP != NULL);
    self->ops->PWM_STOP(self, pwm_id, brake);
}

void PWM_Set(PWM_Base_t *self, pwm_id_t pwm_id, int duty_cycle)
{
    PWM_SET(self, pwm_id, duty_cycle);
}

void PWM_Stop(PWM_Base_t *self, pwm_id_t pwm_id, bool brake)
{
    PWM_STOP(self, pwm_id, brake);
}
