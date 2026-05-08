#ifndef __PWM_PC_H__
#define __PWM_PC_H__

#include <assert.h>
#include <stdbool.h>

typedef enum
{
    PWM_ID_ALL = 0,
    PWM_ID_M1 = 1,
    PWM_ID_M2 = 2,
    PWM_ID_M3 = 3,
    PWM_ID_M4 = 4,
} pwm_id_t;

typedef struct PWM_Base PWM_Base_t;

typedef struct
{
    void (*PWM_SET)(PWM_Base_t *self, pwm_id_t pwm_id, int duty_cycle);
    void (*PWM_STOP)(PWM_Base_t *self, pwm_id_t pwm_id, bool brake);
} PWM_Ops_t;

struct PWM_Base
{
    const char *name;
    const PWM_Ops_t *ops;
};

void PWM_SET(PWM_Base_t *self, pwm_id_t pwm_id, int duty_cycle);
void PWM_STOP(PWM_Base_t *self, pwm_id_t pwm_id, bool brake);

void PWM_Set(PWM_Base_t *self, pwm_id_t pwm_id, int duty_cycle);
void PWM_Stop(PWM_Base_t *self, pwm_id_t pwm_id, bool brake);

#endif
