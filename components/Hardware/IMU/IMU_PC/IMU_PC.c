#include "IMU_PC.h"

void IMU_START(IMU_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->IMU_START != NULL);
    self->ops->IMU_START(self);
}

int IMU_UPDATE(IMU_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->IMU_UPDATE != NULL);
    return self->ops->IMU_UPDATE(self);
}

void IMU_GET_GYRO_RAW(IMU_Base_t *self, int16_t gyro[3])
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->IMU_GET_GYRO_RAW != NULL);
    self->ops->IMU_GET_GYRO_RAW(self, gyro);
}

void IMU_GET_GYRO_RADS(IMU_Base_t *self, float gyro_rads[3])
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->IMU_GET_GYRO_RADS != NULL);
    self->ops->IMU_GET_GYRO_RADS(self, gyro_rads);
}

void IMU_GET_ACCEL_RAW(IMU_Base_t *self, int16_t accel[3])
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->IMU_GET_ACCEL_RAW != NULL);
    self->ops->IMU_GET_ACCEL_RAW(self, accel);
}

void IMU_GET_ACCEL_MS2(IMU_Base_t *self, float accel_ms2[3])
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->IMU_GET_ACCEL_MS2 != NULL);
    self->ops->IMU_GET_ACCEL_MS2(self, accel_ms2);
}
