#include "I2C_PC.h"


void I2C_DELETE(I2C_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->I2C_DELETE != NULL);
    self->ops->I2C_DELETE(self);
}

esp_err_t I2C_READ(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->I2C_READ != NULL);
    return self->ops->I2C_READ(self, addr, reg, len, data);
}

uint8_t I2C_READ_BYTE(I2C_Base_t *self, uint8_t addr, uint8_t reg)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->I2C_READ_BYTE != NULL);
    return self->ops->I2C_READ_BYTE(self, addr, reg);
}

esp_err_t I2C_WRITE(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->I2C_WRITE != NULL);
    return self->ops->I2C_WRITE(self, addr, reg, len, data);
}

esp_err_t I2C_WRITE_BYTE(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint8_t data)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->I2C_WRITE_BYTE != NULL);
    return self->ops->I2C_WRITE_BYTE(self, addr, reg, data);
}
