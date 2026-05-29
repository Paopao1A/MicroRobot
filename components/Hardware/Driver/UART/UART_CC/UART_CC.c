#include "UART_CC.h"

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "Hardware_Common.h"

static const char *TAG = "UART";

static void ESPUART_InitOps(ESPUART_Class_t *self);
static esp_err_t ESPUART_Delete(UART_Base_t *self);
static int ESPUART_ReadBytes(UART_Base_t *self, uint8_t *data, size_t len, uint32_t timeout_ms);
static int ESPUART_WriteBytes(UART_Base_t *self, const uint8_t *data, size_t len);
static esp_err_t ESPUART_GetBufferedLength(UART_Base_t *self, size_t *len);
static esp_err_t ESPUART_Flush(UART_Base_t *self);

static const UART_Ops_t ESP_UART_Ops = {
    .UART_DELETE = ESPUART_Delete,
    .UART_READ_BYTES = ESPUART_ReadBytes,
    .UART_WRITE_BYTES = ESPUART_WriteBytes,
    .UART_GET_BUFFERED_LENGTH = ESPUART_GetBufferedLength,
    .UART_FLUSH = ESPUART_Flush,
};

static void ESPUART_InitOps(ESPUART_Class_t *self)
{
    if (self->installed)
    {
        return;
    }

    ESP_LOGI(TAG,
             "Init UART%d: TX->GPIO%d, RX->GPIO%d, baud %d",
             (int)self->port,
             (int)self->tx_gpio_num,
             (int)self->rx_gpio_num,
             self->baud_rate);

    uart_config_t uart_config = {
        .baud_rate = self->baud_rate, //波特率
        .data_bits = UART_DATA_8_BITS, //数据位
        .parity = UART_PARITY_DISABLE, //校验位
        .stop_bits = UART_STOP_BITS_1, //停止位
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, //硬件流控制
        .source_clk = UART_SCLK_DEFAULT, //时钟源
    };

    ESP_ERROR_CHECK(uart_param_config(self->port, &uart_config));//配置UART参数
    ESP_ERROR_CHECK(uart_set_pin(self->port,     //设置UART引脚
                                 self->tx_gpio_num,
                                 self->rx_gpio_num,
                                 UART_PIN_NO_CHANGE,
                                 UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(self->port,     //安装UART驱动
                                        self->rx_buffer_size,
                                        self->tx_buffer_size,
                                        0,
                                        NULL,
                                        0));
    self->installed = true;
}

//删除UART
static esp_err_t ESPUART_Delete(UART_Base_t *self)
{
    ESPUART_Class_t *class = container_of(self, ESPUART_Class_t, base);

    if (!class->installed)
    {
        return ESP_OK;
    }

    esp_err_t ret = uart_driver_delete(class->port);
    if (ret == ESP_OK)
    {
        class->installed = false;
    }
    return ret;
}

//读取多个字节数据
static int ESPUART_ReadBytes(UART_Base_t *self, uint8_t *data, size_t len, uint32_t timeout_ms)
{
    ESPUART_Class_t *class = container_of(self, ESPUART_Class_t, base);

    if ((data == NULL) || (len == 0))
    {
        return 0;
    }

    return uart_read_bytes(class->port, data, len, pdMS_TO_TICKS(timeout_ms));
}

//写入多个字节数据
static int ESPUART_WriteBytes(UART_Base_t *self, const uint8_t *data, size_t len)
{
    ESPUART_Class_t *class = container_of(self, ESPUART_Class_t, base);

    if ((data == NULL) || (len == 0))
    {
        return 0;
    }

    return uart_write_bytes(class->port, data, len);
}

//获取缓冲区数据长度
static esp_err_t ESPUART_GetBufferedLength(UART_Base_t *self, size_t *len)
{
    ESPUART_Class_t *class = container_of(self, ESPUART_Class_t, base);

    if (len == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    return uart_get_buffered_data_len(class->port, len);
}

//刷新输入缓冲区
static esp_err_t ESPUART_Flush(UART_Base_t *self)
{
    ESPUART_Class_t *class = container_of(self, ESPUART_Class_t, base);
    return uart_flush_input(class->port);
}

void ESPUART_Construct(ESPUART_Class_t *self,
                       const char *name,
                       uart_port_t port,//UART端口号
                       gpio_num_t tx_gpio_num,
                       gpio_num_t rx_gpio_num,
                       int baud_rate,
                       int rx_buffer_size,
                       int tx_buffer_size)
{
    self->base.name = name;
    self->base.ops = &ESP_UART_Ops;
    self->port = port;
    self->tx_gpio_num = tx_gpio_num;
    self->rx_gpio_num = rx_gpio_num;
    self->baud_rate = baud_rate;
    self->rx_buffer_size = rx_buffer_size;
    self->tx_buffer_size = tx_buffer_size;
    self->installed = false;
}

void ESPUART_Init(ESPUART_Class_t *self,
                  const char *name,
                  uart_port_t port,
                  gpio_num_t tx_gpio_num,
                  gpio_num_t rx_gpio_num,
                  int baud_rate,
                  int rx_buffer_size,
                  int tx_buffer_size)
{
    ESPUART_Construct(self,
                      name,
                      port,
                      tx_gpio_num,
                      rx_gpio_num,
                      baud_rate,
                      rx_buffer_size,
                      tx_buffer_size);
    ESPUART_InitOps(self);
}
