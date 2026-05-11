#include "IMU.h"
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
