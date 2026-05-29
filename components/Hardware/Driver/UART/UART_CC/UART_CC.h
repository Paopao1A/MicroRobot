#ifndef __UART_CC_H__
#define __UART_CC_H__

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "UART_PC.h"

#define LIDAR_UART_NUM              UART_NUM_1
#define LIDAR_UART_TX_GPIO          GPIO_NUM_17
#define LIDAR_UART_RX_GPIO          GPIO_NUM_18
#define LIDAR_UART_BAUD_RATE        230400
#define LIDAR_UART_RX_BUFFER_SIZE   2048
#define LIDAR_UART_TX_BUFFER_SIZE   0

typedef struct
{
    UART_Base_t base;
    uart_port_t port;
    gpio_num_t tx_gpio_num;
    gpio_num_t rx_gpio_num;
    int baud_rate;
    int rx_buffer_size;
    int tx_buffer_size;
    bool installed;
} ESPUART_Class_t;

void ESPUART_Construct(ESPUART_Class_t *self,
                       const char *name,
                       uart_port_t port,
                       gpio_num_t tx_gpio_num,
                       gpio_num_t rx_gpio_num,
                       int baud_rate,
                       int rx_buffer_size,
                       int tx_buffer_size);
void ESPUART_Init(ESPUART_Class_t *self,
                  const char *name,
                  uart_port_t port,
                  gpio_num_t tx_gpio_num,
                  gpio_num_t rx_gpio_num,
                  int baud_rate,
                  int rx_buffer_size,
                  int tx_buffer_size);

#endif
