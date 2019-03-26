
#include <msp430.h>
#include <inttypes.h>
#include <string.h>
#include "uart0.h"

// get the UART0_SPEED_ #define
#include "config.h"

volatile char uart0_rx_buf[UART0_RXBUF_SZ];     // receive buffer
volatile uint8_t uart0_p;       // number of characters received, 0 if none
volatile uint8_t uart0_rx_enable;
volatile uint8_t uart0_rx_err;

volatile uint8_t uart0_last_event;

// you'll have to initialize/map uart ports in main()
// or use uart0_port_init() if no mapping is needed
void uart0_init(void)
{
    UCA0CTLW0 = UCSWRST;        // put eUSCI state machine in reset

    // consult 'Recommended Settings for Typical Crystals and Baud Rates' in slau367o
    // for some reason any baud >= 115200 ends up with a non-working RX channel

#ifdef UART0_SPEED_9600_1M
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 6;
    UCA0MCTLW = 0x2081;
#elif defined (UART0_SPEED_19200_1M)
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 3;
    UCA0MCTLW = 0x0241;
#elif defined (UART0_SPEED_38400_1M)
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 1;
    UCA0MCTLW = 0x00a1;
#elif defined (UART0_SPEED_57600_1M)
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 17;
    UCA0MCTLW = 0x4a00;
#elif defined (UART0_SPEED_115200_1M)
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 8;
    UCA0MCTLW = 0xd600;
#elif defined (UART0_SPEED_9600_8M)
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 52;
    UCA0MCTLW = 0x4911;
#elif defined (UART0_SPEED_19200_8M)
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 26;
    UCA0MCTLW = 0xb601;
#elif defined (UART0_SPEED_38400_8M)
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 13;
    UCA0MCTLW = 0x8401;
#elif defined (UART0_SPEED_57600_8M)
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 8;
    UCA0MCTLW = 0xf7a1;
#elif defined (UART0_SPEED_115200_8M)
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 4;
    UCA0MCTLW = 0x5551;
#elif defined (UART0_SPEED_230400_8M)
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 2;
    UCA0MCTLW = 0xbb21;
#elif defined (UART0_SPEED_460800_8M)
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 17;
    UCA0MCTLW = 0x4a00;
#else // a safer default of 9600 - does not depend on SMCLK
    UCA0CTLW0 |= UCSSEL__ACLK;
    UCA0BRW = 3;
    //UCA0MCTLW |= 0x5300;
    UCA0MCTLW |= 0x9200;
#endif

    UCA0CTLW0 &= ~UCSWRST;      // Initialize eUSCI
    UCA0IE |= UCRXIE;           // Enable USCI_A0 RX interrupt

    uart0_p = 0;
    uart0_rx_enable = 1;
    uart0_rx_err = 0;
}

// default port locations
void uart0_port_init(void)
{
    P2SEL0 &= ~(BIT0 | BIT1);
    P2SEL1 |= (BIT0 | BIT1);
}

uint8_t uart0_get_event(void)
{
    return uart0_last_event;
}

void uart0_rst_event(void)
{
    uart0_last_event = UART0_EV_NULL;
}

void uart0_set_eol(void)
{
    uart0_p = 0;
    uart0_rx_enable = 1;
}

char *uart0_get_rx_buf(void)
{
    if (uart0_p) {
        return (char *)uart0_rx_buf;
    } else {
        return NULL;
    }
}

uint16_t uart0_tx_str(const char *str, const uint16_t size)
{
    uint16_t p = 0;
    while (p < size) {
        while (!(UCA0IFG & UCTXIFG)) {
        }                       // USCI_A0 TX buffer ready?
        UCA0TXBUF = str[p];
        p++;
    }
    return p;
}

uint16_t uart0_print(const char *str)
{
    size_t p = 0;
    size_t size = strlen(str);
    while (p < size) {
        while (!(UCA0IFG & UCTXIFG)) {
        }                       // USCI_A0 TX buffer ready?
        UCA0TXBUF = str[p];
        p++;
    }
    return p;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=EUSCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(EUSCI_A0_VECTOR))) USCI_A0_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = UCA0IV;
    register char rx;
    uint8_t ev = 0;

    switch (iv) {
    case USCI_UART_UCRXIFG:
        rx = UCA0RXBUF;

        if (rx == 0x0a) {
            return;
        }

        if (uart0_rx_enable && (!uart0_rx_err) && (uart0_p < UART0_RXBUF_SZ)) {
            if (rx == 0x0d) {
                uart0_rx_buf[uart0_p] = 0;
                uart0_rx_enable = 0;
                uart0_rx_err = 0;
                if (uart0_p) {
                    ev = UART0_EV_RX;
                    _BIC_SR_IRQ(LPM3_bits);
                }
            } else {
                uart0_rx_buf[uart0_p] = rx;
                uart0_p++;
            }
        } else {
            uart0_rx_err++;
            uart0_p = 0;
            if ((rx == 0x0d) || (rx == 0x0a)) {
                uart0_rx_err = 0;
                uart0_rx_enable = 1;
            }
        }
        break;
    case USCI_UART_UCTXIFG:
        ev = UART0_EV_TX;
        break;
    default:
        break;
    }
    uart0_last_event |= ev;
}
