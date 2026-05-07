#ifndef __I2C_PC_H__
#define __I2C_PC_H__

#include <assert.h>
#include <stdint.h>

#include "esp_err.h"

typedef struct I2C_Base I2C_Base_t;

typedef struct
{
    void (*I2C_INIT)(I2C_Base_t *self);
    void (*I2C_DELETE)(I2C_Base_t *self);
    esp_err_t (*I2C_READ)(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data);
    uint8_t (*I2C_READ_BYTE)(I2C_Base_t *self, uint8_t addr, uint8_t reg);
    esp_err_t (*I2C_WRITE)(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data);
    esp_err_t (*I2C_WRITE_BYTE)(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint8_t data);
} I2C_Ops_t;

struct I2C_Base
{
    const char *name;
    const I2C_Ops_t *ops;
};

void I2C_INIT(I2C_Base_t *self);
void I2C_DELETE(I2C_Base_t *self);
esp_err_t I2C_READ(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data);
uint8_t I2C_READ_BYTE(I2C_Base_t *self, uint8_t addr, uint8_t reg);
esp_err_t I2C_WRITE(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data);
esp_err_t I2C_WRITE_BYTE(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint8_t data);

void I2C_Init(I2C_Base_t *self);
void I2C_Delete(I2C_Base_t *self);
esp_err_t I2C_Read(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data);
uint8_t I2C_ReadByte(I2C_Base_t *self, uint8_t addr, uint8_t reg);
esp_err_t I2C_Write(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data);
esp_err_t I2C_WriteByte(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint8_t data);

#endif
