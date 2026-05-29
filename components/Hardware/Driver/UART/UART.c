#include "UART.h"

#include "BOARD.h"

static ESPUART_Class_t s_legacy_uart;

static UART_Base_t *UART_Get_Default(void)
{
    if (Esp_UART == NULL)
    {
        ESPUART_Init(&s_legacy_uart,
                     "lidar_uart",
                     LIDAR_UART_NUM,
                     LIDAR_UART_TX_GPIO,
                     LIDAR_UART_RX_GPIO,
                     LIDAR_UART_BAUD_RATE,
                     LIDAR_UART_RX_BUFFER_SIZE,
                     LIDAR_UART_TX_BUFFER_SIZE);
        Esp_UART = &s_legacy_uart.base;
    }

    return Esp_UART;
}

esp_err_t UART_Delete(void)
{
    return UART_DELETE(UART_Get_Default());
}

int UART_ReadBytes(uint8_t *data, size_t len, uint32_t timeout_ms)
{
    return UART_READ_BYTES(UART_Get_Default(), data, len, timeout_ms);
}

int UART_WriteBytes(const uint8_t *data, size_t len)
{
    return UART_WRITE_BYTES(UART_Get_Default(), data, len);
}

esp_err_t UART_GetBufferedLength(size_t *len)
{
    return UART_GET_BUFFERED_LENGTH(UART_Get_Default(), len);
}

esp_err_t UART_Flush(void)
{
    return UART_FLUSH(UART_Get_Default());
}
