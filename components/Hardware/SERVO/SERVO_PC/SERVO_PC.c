#include "SERVO_PC.h"

void SERVO_SET_ANGLE(SERVO_Base_t *self, servo_id_t servo_id, float angle_deg)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->SERVO_SET_ANGLE != NULL);
    self->ops->SERVO_SET_ANGLE(self, servo_id, angle_deg);
}

void SERVO_SET_PULSE_US(SERVO_Base_t *self, servo_id_t servo_id, uint32_t pulse_us)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->SERVO_SET_PULSE_US != NULL);
    self->ops->SERVO_SET_PULSE_US(self, servo_id, pulse_us);
}

void SERVO_SetAngle(SERVO_Base_t *self, servo_id_t servo_id, float angle_deg)
{
    SERVO_SET_ANGLE(self, servo_id, angle_deg);
}

void SERVO_SetPulseUs(SERVO_Base_t *self, servo_id_t servo_id, uint32_t pulse_us)
{
    SERVO_SET_PULSE_US(self, servo_id, pulse_us);
}
