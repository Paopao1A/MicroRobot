#ifndef __PWM_H__
#define __PWM_H__

#include <stdbool.h>

#include "PWM_CC.h"
#include "PWM_PC.h"

void PWM_Set_Duty(pwm_id_t pwm_id, int duty_cycle);
void PWM_Stop_Motor(pwm_id_t pwm_id, bool brake);

#endif
