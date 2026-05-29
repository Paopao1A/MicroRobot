#include "UART_PC.h"

esp_err_t UART_DELETE(UART_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->UART_DELETE != NULL);
    return self->ops->UART_DELETE(self);
}

int UART_READ_BYTES(UART_Base_t *self, uint8_t *data, size_t len, uint32_t timeout_ms)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->UART_READ_BYTES != NULL);
    return self->ops->UART_READ_BYTES(self, data, len, timeout_ms);
}

int UART_WRITE_BYTES(UART_Base_t *self, const uint8_t *data, size_t len)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->UART_WRITE_BYTES != NULL);
    return self->ops->UART_WRITE_BYTES(self, data, len);
}

esp_err_t UART_GET_BUFFERED_LENGTH(UART_Base_t *self, size_t *len)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->UART_GET_BUFFERED_LENGTH != NULL);
    return self->ops->UART_GET_BUFFERED_LENGTH(self, len);
}

esp_err_t UART_FLUSH(UART_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->UART_FLUSH != NULL);
    return self->ops->UART_FLUSH(self);
}
