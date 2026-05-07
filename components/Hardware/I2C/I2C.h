#ifndef __I2C_H__
#define __I2C_H__

#include "I2C_CC.h"
#include "I2C_PC.h"

void I2C_Master_Init(void);
void I2C_Master_Delete(void);

esp_err_t I2C_Master_Read(uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data);
uint8_t I2C_Master_Read_Byte(uint8_t addr, uint8_t reg);

esp_err_t I2C_Master_Write(uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data);
esp_err_t I2C_Master_Write_Byte(uint8_t addr, uint8_t reg, uint8_t data);

#endif

