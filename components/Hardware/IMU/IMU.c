#include "IMU.h"

#include <stddef.h>

#include "BOARD.h"

static ESPIMU_Class_t s_legacy_imu;

static IMU_Base_t *IMU_Get_Default(void)
{
    if (Esp_IMU == NULL)
    {
        ESPIMU_Init(&s_legacy_imu, "icm42670p", ICM42670P_I2C_ADDR);
        Esp_IMU = &s_legacy_imu.base;
    }

    return Esp_IMU;
}

float IMU_Get_Angular(void)//获取Z轴角速度（弧度每秒）
{
    IMU_UPDATE(IMU_Get_Default());
    float gyro_rads[3] = {0.0f};
    IMU_GET_GYRO_RADS(IMU_Get_Default(), gyro_rads);
    return gyro_rads[2];
}

void IMU_Get_GyroRaw(int16_t gyro[3])//获取陀螺仪原始数据
{
    if (gyro == NULL)
    {
        return;
    }

    IMU_UPDATE(IMU_Get_Default());
    IMU_GET_GYRO_RAW(IMU_Get_Default(), gyro);
}

void IMU_Get_GyroRads(float gyro_rads[3])//获取陀螺仪角速度数据（弧度每秒）
{
    if (gyro_rads == NULL)
    {
        return;
    }

    IMU_UPDATE(IMU_Get_Default());
    IMU_GET_GYRO_RADS(IMU_Get_Default(), gyro_rads);
}

void IMU_Get_AccelRaw(int16_t accel[3])//获取加速度原始数据
{
    if (accel == NULL)
    {
        return;
    }

    IMU_UPDATE(IMU_Get_Default());
    IMU_GET_ACCEL_RAW(IMU_Get_Default(), accel);
}

void IMU_Get_AccelMs2(float accel_ms2[3])//获取加速度数据（米每二次方秒）
{
    if (accel_ms2 == NULL)
    {
        return;
    }

    IMU_UPDATE(IMU_Get_Default());
    IMU_GET_ACCEL_MS2(IMU_Get_Default(), accel_ms2);
}

int IMU_Update_All(int16_t gyro_raw[3], float gyro_rads[3], int16_t accel_raw[3], float accel_ms2[3])
{
    IMU_Base_t *imu = IMU_Get_Default();
    int ret = IMU_UPDATE(imu);

    if (ret != 0)
    {
        return ret;
    }

    if (gyro_raw != NULL)
    {
        IMU_GET_GYRO_RAW(imu, gyro_raw);
    }
    if (gyro_rads != NULL)
    {
        IMU_GET_GYRO_RADS(imu, gyro_rads);
    }
    if (accel_raw != NULL)
    {
        IMU_GET_ACCEL_RAW(imu, accel_raw);
    }
    if (accel_ms2 != NULL)
    {
        IMU_GET_ACCEL_MS2(imu, accel_ms2);
    }

    return ret;
}
