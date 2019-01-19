#ifndef __UART0_H__
#define __UART0_H__

#include <stdint.h>

enum uart0_tevent {
    UART0_EV_NONE = 0,
    UART0_EV_RX   = 0x1,
    UART0_EV_TX   = 0x2
};

#define UART0_RXBUF_SZ      64

volatile char uart0_rx_buf[UART0_RXBUF_SZ];
volatile uint8_t uart0_p;
volatile uint8_t uart0_rx_enable;
volatile uint8_t uart0_rx_err;

void uart0_init();
void uart0_port_init();
uint16_t uart0_tx_str(char *str, const uint16_t size);

volatile enum uart0_tevent uart0_last_event;

#endif
