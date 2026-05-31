#ifndef __SERVO_PC_H__
#define __SERVO_PC_H__

#include <assert.h>
#include <stdint.h>

typedef enum
{
    SERVO_ID_ALL = 0,
    SERVO_ID_PAN = 1,
    SERVO_ID_TILT = 2,
} servo_id_t;

typedef struct SERVO_Base SERVO_Base_t;

typedef struct
{
    void (*SERVO_SET_ANGLE)(SERVO_Base_t *self, servo_id_t servo_id, float angle_deg);
    void (*SERVO_SET_PULSE_US)(SERVO_Base_t *self, servo_id_t servo_id, uint32_t pulse_us);
} SERVO_Ops_t;

struct SERVO_Base
{
    const char *name;
    const SERVO_Ops_t *ops;
};

void SERVO_SET_ANGLE(SERVO_Base_t *self, servo_id_t servo_id, float angle_deg);
void SERVO_SET_PULSE_US(SERVO_Base_t *self, servo_id_t servo_id, uint32_t pulse_us);

void SERVO_SetAngle(SERVO_Base_t *self, servo_id_t servo_id, float angle_deg);
void SERVO_SetPulseUs(SERVO_Base_t *self, servo_id_t servo_id, uint32_t pulse_us);

#endif
