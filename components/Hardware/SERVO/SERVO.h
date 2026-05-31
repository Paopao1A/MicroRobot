#ifndef __SERVO_H__
#define __SERVO_H__

#include <stdint.h>

#include "SERVO_CC.h"
#include "SERVO_PC.h"

void SERVO_Set_Angle(servo_id_t servo_id, float angle_deg);
void SERVO_Set_PulseUs(servo_id_t servo_id, uint32_t pulse_us);

#endif
