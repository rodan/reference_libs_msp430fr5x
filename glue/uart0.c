
#include "uart0.h"

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
    UCA0CTLW0 |= UCSSEL__ACLK;  // CLK = ACLK
    UCA0BRW = 3;                // 32kHz/3 ~ 9600 baud
    UCA0MCTLW |= 0x5300;        // 32768/9600 - INT(32768/9600)=0.41
    // UCBRSx value = 0x53 (See UG)
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

uint16_t uart0_tx_str(char *str, const uint16_t size)
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
