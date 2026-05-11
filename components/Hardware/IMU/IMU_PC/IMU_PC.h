#ifndef __IMU_PC_H__
#define __IMU_PC_H__

#include <assert.h>
#include <stdint.h>

typedef struct IMU_Base IMU_Base_t;

typedef struct
{
    void (*IMU_START)(IMU_Base_t *self);
    int (*IMU_UPDATE)(IMU_Base_t *self);
    void (*IMU_GET_GYRO_RAW)(IMU_Base_t *self, int16_t gyro[3]);
    void (*IMU_GET_GYRO_RADS)(IMU_Base_t *self, float gyro_rads[3]);
} IMU_Ops_t;

struct IMU_Base
{
    const char *name;
    const IMU_Ops_t *ops;
};

void IMU_START(IMU_Base_t *self);
int IMU_UPDATE(IMU_Base_t *self);
void IMU_GET_GYRO_RAW(IMU_Base_t *self, int16_t gyro[3]);
void IMU_GET_GYRO_RADS(IMU_Base_t *self, float gyro_rads[3]);

#endif
