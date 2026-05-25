#include "IMU_CC.h"

#include <math.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Hardware_Common.h"
#include "I2C.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const char *TAG = "ICM42670P";

static void ESPIMU_Start(IMU_Base_t *self);
static int ESPIMU_Update(IMU_Base_t *self);
static void ESPIMU_GetGyroRaw(IMU_Base_t *self, int16_t gyro[3]);
static void ESPIMU_GetGyroRads(IMU_Base_t *self, float gyro_rads[3]);
static void ESPIMU_GetAccelRaw(IMU_Base_t *self, int16_t accel[3]);
static void ESPIMU_GetAccelMs2(IMU_Base_t *self, float accel_ms2[3]);

static const IMU_Ops_t ESP_IMU_Ops = {
    .IMU_START = ESPIMU_Start,
    .IMU_UPDATE = ESPIMU_Update,
    .IMU_GET_GYRO_RAW = ESPIMU_GetGyroRaw,
    .IMU_GET_GYRO_RADS = ESPIMU_GetGyroRads,
    .IMU_GET_ACCEL_RAW = ESPIMU_GetAccelRaw,
    .IMU_GET_ACCEL_MS2 = ESPIMU_GetAccelMs2,
};

static int ESPIMU_ReadReg(ESPIMU_Class_t *class, uint8_t reg, uint8_t *data, uint16_t len)
{
    return I2C_Master_Read(class->i2c_addr, reg, len, data);
}

static int ESPIMU_WriteReg(ESPIMU_Class_t *class, uint8_t reg, uint8_t data)
{
    return I2C_Master_Write_Byte(class->i2c_addr, reg, data);
}

static void ESPIMU_Start(IMU_Base_t *self)
{
    ESPIMU_Class_t *class = container_of(self, ESPIMU_Class_t, base);
    uint8_t who_am_i = 0;


    if (ESPIMU_ReadReg(class, ICM42670P_REG_WHO_AM_I, &who_am_i, 1) != ESP_OK)
    {
        class->start_ok = -1;
        ESP_LOGE(TAG, "Read WHO_AM_I failed");
        return;
    }

    if (who_am_i != ICM42670P_WHO_AM_I_VALUE)
    {
        class->start_ok = -1;
        ESP_LOGE(TAG, "Bad WHO_AM_I: read 0x%02x, expected 0x%02x", who_am_i, ICM42670P_WHO_AM_I_VALUE);
        return;
    }

    ESPIMU_WriteReg(class, ICM42670P_REG_GYRO_CONFIG0, ICM42670P_GYRO_CONFIG_400HZ);
    ESPIMU_WriteReg(class, ICM42670P_REG_ACCEL_CONFIG0, ICM42670P_ACCEL_CONFIG_400HZ);
    ESPIMU_WriteReg(class, ICM42670P_REG_PWR_MGMT0, ICM42670P_ACCEL_GYRO_MODE_LN);
    vTaskDelay(pdMS_TO_TICKS(50));

    class->start_ok = 1;
    ESP_LOGI(TAG, "ICM42670P init OK");
}


static int ESPIMU_Update(IMU_Base_t *self)
{
    ESPIMU_Class_t *class = container_of(self, ESPIMU_Class_t, base);
    uint8_t data[12] = {0};

    if (class->start_ok <= 0)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (ESPIMU_ReadReg(class, ICM42670P_REG_ACCEL_DATA_X1, data, sizeof(data)) != ESP_OK)
    {
        return ESP_FAIL;
    }

    class->accel_raw[0] = (int16_t)((data[0] << 8) | data[1]);
    class->accel_raw[1] = (int16_t)((data[2] << 8) | data[3]);
    class->accel_raw[2] = (int16_t)((data[4] << 8) | data[5]);
    class->gyro_raw[0] = (int16_t)((data[6] << 8) | data[7]);
    class->gyro_raw[1] = (int16_t)((data[8] << 8) | data[9]);
    class->gyro_raw[2] = (int16_t)((data[10] << 8) | data[11]);

    class->accel_ms2[0] = (float)class->accel_raw[0] * class->accel_scale_ms2_per_lsb;
    class->accel_ms2[1] = (float)class->accel_raw[1] * class->accel_scale_ms2_per_lsb;
    class->accel_ms2[2] = (float)class->accel_raw[2] * class->accel_scale_ms2_per_lsb;
    class->gyro_rads[0] = (float)class->gyro_raw[0] * class->gyro_scale_rads_per_lsb;
    class->gyro_rads[1] = (float)class->gyro_raw[1] * class->gyro_scale_rads_per_lsb;
    class->gyro_rads[2] = (float)class->gyro_raw[2] * class->gyro_scale_rads_per_lsb;

    return ESP_OK;
}

static void ESPIMU_GetGyroRaw(IMU_Base_t *self, int16_t gyro[3])
{
    ESPIMU_Class_t *class = container_of(self, ESPIMU_Class_t, base);
    gyro[0] = class->gyro_raw[0];
    gyro[1] = class->gyro_raw[1];
    gyro[2] = class->gyro_raw[2];
}

static void ESPIMU_GetGyroRads(IMU_Base_t *self, float gyro_rads[3])
{
    ESPIMU_Class_t *class = container_of(self, ESPIMU_Class_t, base);
    gyro_rads[0] = class->gyro_rads[0];
    gyro_rads[1] = class->gyro_rads[1];
    gyro_rads[2] = class->gyro_rads[2];
}

static void ESPIMU_GetAccelRaw(IMU_Base_t *self, int16_t accel[3])
{
    ESPIMU_Class_t *class = container_of(self, ESPIMU_Class_t, base);
    accel[0] = class->accel_raw[0];
    accel[1] = class->accel_raw[1];
    accel[2] = class->accel_raw[2];
}

static void ESPIMU_GetAccelMs2(IMU_Base_t *self, float accel_ms2[3])
{
    ESPIMU_Class_t *class = container_of(self, ESPIMU_Class_t, base);
    accel_ms2[0] = class->accel_ms2[0];
    accel_ms2[1] = class->accel_ms2[1];
    accel_ms2[2] = class->accel_ms2[2];
}

void ESPIMU_Init(ESPIMU_Class_t *self, const char *name, uint8_t i2c_addr)
{
    self->base.name = name;
    self->base.ops = &ESP_IMU_Ops;
    self->i2c_addr = i2c_addr;
    self->start_ok = 0;
    self->accel_fsr_g = ICM42670P_ACCEL_FSR_G;//加速度满量程4g，因此数值为4
    self->gyro_fsr_dps = ICM42670P_GYRO_FSR_DPS;//陀螺仪满量程2000 dps，因此数值为2000
    self->accel_scale_ms2_per_lsb = ((float)self->accel_fsr_g * ICM42670P_GRAVITY_MS2) / ICM42670P_ACCEL_RAW_FULL_SCALE;
    self->gyro_scale_rads_per_lsb = ((float)self->gyro_fsr_dps * (float)M_PI / 180.0f) / ICM42670P_GYRO_RAW_FULL_SCALE;
    for (uint8_t i = 0; i < 3; i++)
    {
        self->accel_raw[i] = 0;
        self->accel_ms2[i] = 0.0f;
        self->gyro_raw[i] = 0;
        self->gyro_rads[i] = 0.0f;
    }

    ESPIMU_Start(&self->base);
}
