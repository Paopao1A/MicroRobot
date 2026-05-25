#ifndef __IMU_H__
#define __IMU_H__

#include "IMU_CC.h"
#include "IMU_PC.h"

float IMU_Get_Angular(void);
void IMU_Get_GyroRaw(int16_t gyro[3]);
void IMU_Get_GyroRads(float gyro_rads[3]);
void IMU_Get_AccelRaw(int16_t accel[3]);
void IMU_Get_AccelMs2(float accel_ms2[3]);
int IMU_Update_All(int16_t gyro_raw[3], float gyro_rads[3], int16_t accel_raw[3], float accel_ms2[3]);


#endif
