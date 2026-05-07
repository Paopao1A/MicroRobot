#include "I2C_PC.h"

void I2C_Init(I2C_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->I2C_Init != NULL);
    self->ops->I2C_Init(self);
}

void I2C_Delete(I2C_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->I2C_Delete != NULL);
    self->ops->I2C_Delete(self);
}

esp_err_t I2C_Read(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->I2C_Read != NULL);
    return self->ops->I2C_Read(self, addr, reg, len, data);
}

uint8_t I2C_ReadByte(I2C_Base_t *self, uint8_t addr, uint8_t reg)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->I2C_ReadByte != NULL);
    return self->ops->I2C_ReadByte(self, addr, reg);
}

esp_err_t I2C_Write(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->I2C_Write != NULL);
    return self->ops->I2C_Write(self, addr, reg, len, data);
}

esp_err_t I2C_WriteByte(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint8_t data)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->I2C_WriteByte != NULL);
    return self->ops->I2C_WriteByte(self, addr, reg, data);
}

