#ifndef __UART_PC_H__
#define __UART_PC_H__

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

typedef struct UART_Base UART_Base_t;

typedef struct
{
    esp_err_t (*UART_DELETE)(UART_Base_t *self);
    int (*UART_READ_BYTES)(UART_Base_t *self, uint8_t *data, size_t len, uint32_t timeout_ms);
    int (*UART_WRITE_BYTES)(UART_Base_t *self, const uint8_t *data, size_t len);
    esp_err_t (*UART_GET_BUFFERED_LENGTH)(UART_Base_t *self, size_t *len);
    esp_err_t (*UART_FLUSH)(UART_Base_t *self);
} UART_Ops_t;

struct UART_Base
{
    const char *name;
    const UART_Ops_t *ops;
};

esp_err_t UART_DELETE(UART_Base_t *self);
int UART_READ_BYTES(UART_Base_t *self, uint8_t *data, size_t len, uint32_t timeout_ms);
int UART_WRITE_BYTES(UART_Base_t *self, const uint8_t *data, size_t len);
esp_err_t UART_GET_BUFFERED_LENGTH(UART_Base_t *self, size_t *len);
esp_err_t UART_FLUSH(UART_Base_t *self);

#endif
