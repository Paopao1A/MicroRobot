#ifndef __UART_H__
#define __UART_H__

#include <stddef.h>
#include <stdint.h>

#include "UART_CC.h"
#include "UART_PC.h"

esp_err_t UART_Delete(void);
int UART_ReadBytes(uint8_t *data, size_t len, uint32_t timeout_ms);
int UART_WriteBytes(const uint8_t *data, size_t len);
esp_err_t UART_GetBufferedLength(size_t *len);
esp_err_t UART_Flush(void);

#endif
