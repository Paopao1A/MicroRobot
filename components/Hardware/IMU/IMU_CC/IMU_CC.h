#ifndef __IMU_CC_H__
#define __IMU_CC_H__

#include <stdint.h>

#include "IMU_PC.h"

#define ICM42670P_I2C_ADDR              0x68    //陀螺仪I2C地址
#define ICM42670P_WHO_AM_I_VALUE        0x67    //陀螺仪ID值

#define ICM42670P_REG_ACCEL_DATA_X1     0x0B    //加速度X轴数据寄存器1
#define ICM42670P_REG_GYRO_DATA_X1      0x11    //陀螺仪X轴数据速度寄存器1
#define ICM42670P_REG_PWR_MGMT0         0x1F    //陀螺仪电源管理寄存器0
#define ICM42670P_REG_GYRO_CONFIG0      0x20    //陀螺仪配置寄存器0
#define ICM42670P_REG_ACCEL_CONFIG0     0x21    //加速度配置寄存器0
#define ICM42670P_REG_WHO_AM_I          0x75    //陀螺仪ID寄存器

#define ICM42670P_ACCEL_FSR_G           4
#define ICM42670P_GYRO_FSR_DPS          2000    //陀螺仪满量程2000 dps寄存器值
#define ICM42670P_ACCEL_RAW_FULL_SCALE  32768.0f  //加速度原始数据满量程范围
#define ICM42670P_GYRO_RAW_FULL_SCALE   32768.0f //陀螺仪原始数据满量程范围
#define ICM42670P_GRAVITY_MS2           9.80665f //标准重力加速度

#define ICM42670P_GYRO_CONFIG_400HZ     0x07    //陀螺仪400Hz模式配置寄存器值
#define ICM42670P_ACCEL_CONFIG_400HZ    0x47    //加速度400Hz模式配置寄存器值（满量程4g，带宽50Hz）
#define ICM42670P_ACCEL_GYRO_MODE_LN    0x0F    //加速度陀螺仪低功耗模式配置寄存器值

typedef struct
{
    IMU_Base_t base;
    uint8_t i2c_addr;
    int start_ok;
    int16_t accel_raw[3];             //加速度原始数据
    float accel_ms2[3];               //加速度数据（米每二次方秒）
    int16_t gyro_raw[3];             //陀螺仪原始数据
    float gyro_rads[3];              //陀螺仪角速度数据（弧度每秒）
    uint16_t accel_fsr_g;             //加速度满量程4g寄存器值
    uint16_t gyro_fsr_dps;           //陀螺仪满量程2000 dps寄存器值
    float accel_scale_ms2_per_lsb;   //加速度满量程4g对应米每二次方秒
    float gyro_scale_rads_per_lsb;   //陀螺仪满量程2000 dps对应弧度每秒
} ESPIMU_Class_t;

void ESPIMU_Init(ESPIMU_Class_t *self, const char *name, uint8_t i2c_addr);

#endif
