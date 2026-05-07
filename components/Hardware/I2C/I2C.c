#include "I2C.h"
#include "BOARD.h"

static ESPI2C_Class_t s_legacy_i2c;

static I2C_Base_t *I2C_Get_Default(void)
{
    if (Esp_I2C == NULL)
    {
        ESPI2C_Construct(&s_legacy_i2c,
                         "esp_i2c",
                         I2C_MASTER_NUM,
                         I2C_MASTER_GPIO_SCL,
                         I2C_MASTER_GPIO_SDA,
                         I2C_MASTER_FREQ_HZ,
                         I2C_MASTER_TIMEOUT_MS);
        Esp_I2C = &s_legacy_i2c.base;
    }

    return Esp_I2C;
}

void I2C_Master_Init(void)
{
    ESPI2C_Init(&s_legacy_i2c,
                "esp_i2c",
                I2C_MASTER_NUM,
                I2C_MASTER_GPIO_SCL,
                I2C_MASTER_GPIO_SDA,
                I2C_MASTER_FREQ_HZ,
                I2C_MASTER_TIMEOUT_MS);
    Esp_I2C = &s_legacy_i2c.base;
}

void I2C_Master_Delete(void)
{
    I2C_Delete(I2C_Get_Default());
}

esp_err_t I2C_Master_Read(uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data)
{
    return I2C_Read(I2C_Get_Default(), addr, reg, len, data);
}

uint8_t I2C_Master_Read_Byte(uint8_t addr, uint8_t reg)
{
    return I2C_ReadByte(I2C_Get_Default(), addr, reg);
}

esp_err_t I2C_Master_Write(uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data)
{
    return I2C_Write(I2C_Get_Default(), addr, reg, len, data);
}

esp_err_t I2C_Master_Write_Byte(uint8_t addr, uint8_t reg, uint8_t data)
{
    return I2C_WriteByte(I2C_Get_Default(), addr, reg, data);
}

