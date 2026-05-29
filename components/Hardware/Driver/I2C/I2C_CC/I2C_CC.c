#include "I2C_CC.h"

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "Hardware_Common.h"

static const char *TAG = "I2C_MASTER";

static void ESPI2C_InitOps(ESPI2C_Class_t *self);
static void ESPI2C_Delete(I2C_Base_t *self);
static esp_err_t ESPI2C_Read(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data);
static uint8_t ESPI2C_ReadByte(I2C_Base_t *self, uint8_t addr, uint8_t reg);
static esp_err_t ESPI2C_Write(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data);
static esp_err_t ESPI2C_WriteByte(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint8_t data);

static const I2C_Ops_t ESP_I2C_Ops = {
    .I2C_DELETE = ESPI2C_Delete,
    .I2C_READ = ESPI2C_Read,
    .I2C_READ_BYTE = ESPI2C_ReadByte,
    .I2C_WRITE = ESPI2C_Write,
    .I2C_WRITE_BYTE = ESPI2C_WriteByte,
};

static void ESPI2C_InitOps(ESPI2C_Class_t *self)
{

    if (self->installed)
    {
        return;
    }

    ESP_LOGI(TAG, "Init I2C master:SCL->GPIO%d, SDA->GPIO%d", self->scl_gpio_num, self->sda_gpio_num);

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = self->sda_gpio_num,
        .scl_io_num = self->scl_gpio_num,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = self->freq_hz,
    };
    ESP_ERROR_CHECK(i2c_param_config(self->port, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(self->port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
    self->installed = true;
}

static void ESPI2C_Delete(I2C_Base_t *self)
{
    ESPI2C_Class_t *class = container_of(self, ESPI2C_Class_t, base);

    if (!class->installed)
    {
        return;
    }

    ESP_ERROR_CHECK(i2c_driver_delete(class->port));
    class->installed = false;
}

static esp_err_t ESPI2C_Read(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data)
{
    ESPI2C_Class_t *class = container_of(self, ESPI2C_Class_t, base);
    return i2c_master_write_read_device(class->port, addr, &reg, 1, data, len, class->timeout_ms / portTICK_PERIOD_MS);
}

static uint8_t ESPI2C_ReadByte(I2C_Base_t *self, uint8_t addr, uint8_t reg)
{
    uint8_t data = 0;
    ESPI2C_Read(self, addr, reg, 1, &data);
    return data;
}

static esp_err_t ESPI2C_Write(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint16_t len, uint8_t *data)
{
    ESPI2C_Class_t *class = container_of(self, ESPI2C_Class_t, base);
    uint8_t *buf = (uint8_t *)malloc(len + 1);
    esp_err_t ret = ESP_ERR_NO_MEM;

    if (buf == NULL)
    {
        return ret;
    }

    buf[0] = reg;
    for (int i = 0; i < len; i++)
    {
        buf[i + 1] = data[i];
    }

    ret = i2c_master_write_to_device(class->port, addr, buf, len + 1, class->timeout_ms / portTICK_PERIOD_MS);
    free(buf);
    return ret;
}

static esp_err_t ESPI2C_WriteByte(I2C_Base_t *self, uint8_t addr, uint8_t reg, uint8_t data)
{
    ESPI2C_Class_t *class = container_of(self, ESPI2C_Class_t, base);
    uint8_t write_buf[] = {reg, data};
    return i2c_master_write_to_device(class->port, addr, write_buf, sizeof(write_buf), class->timeout_ms / portTICK_PERIOD_MS);
}

void ESPI2C_Construct(ESPI2C_Class_t *self,
                      const char *name,
                      i2c_port_t port,
                      int scl_gpio_num,
                      int sda_gpio_num,
                      uint32_t freq_hz,
                      uint32_t timeout_ms)
{
    self->base.name = name;
    self->base.ops = &ESP_I2C_Ops;
    self->port = port;
    self->scl_gpio_num = scl_gpio_num;
    self->sda_gpio_num = sda_gpio_num;
    self->freq_hz = freq_hz;
    self->timeout_ms = timeout_ms;
    self->installed = false;
}

void ESPI2C_Init(ESPI2C_Class_t *self,
                 const char *name,
                 i2c_port_t port,
                 int scl_gpio_num,
                 int sda_gpio_num,
                 uint32_t freq_hz,
                 uint32_t timeout_ms)
{
    ESPI2C_Construct(self, name, port, scl_gpio_num, sda_gpio_num, freq_hz, timeout_ms);
    ESPI2C_InitOps(self);
}
