#ifndef __I2C_CC_H__
#define __I2C_CC_H__

#include <stdbool.h>
#include <stdint.h>

#include "driver/i2c.h"
#include "I2C_PC.h"

#define I2C_MASTER_GPIO_SCL         39
#define I2C_MASTER_GPIO_SDA         40
#define I2C_MASTER_NUM              0
#define I2C_MASTER_FREQ_HZ          400000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       1000

typedef struct
{
    I2C_Base_t base;
    i2c_port_t port;
    int scl_gpio_num;
    int sda_gpio_num;
    uint32_t freq_hz;
    uint32_t timeout_ms;
    bool installed;
} ESPI2C_Class_t;

void ESPI2C_Construct(ESPI2C_Class_t *self,
                      const char *name,
                      i2c_port_t port,
                      int scl_gpio_num,
                      int sda_gpio_num,
                      uint32_t freq_hz,
                      uint32_t timeout_ms);
void ESPI2C_Init(ESPI2C_Class_t *self,
                 const char *name,
                 i2c_port_t port,
                 int scl_gpio_num,
                 int sda_gpio_num,
                 uint32_t freq_hz,
                 uint32_t timeout_ms);

#endif

