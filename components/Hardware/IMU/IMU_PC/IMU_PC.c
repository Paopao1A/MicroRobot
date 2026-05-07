#include "IMU_PC.h"

void IMU_START(IMU_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->IMU_START != NULL);
    self->ops->IMU_START(self);
}

int IMU_START_OK(IMU_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->IMU_START_OK != NULL);
    return self->ops->IMU_START_OK(self);
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

