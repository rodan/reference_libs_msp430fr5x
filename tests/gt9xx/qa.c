
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "qa.h"
#include "timer_a0.h"
#include "timer_a1.h"
#include "gt9xx.h"

void display_menu(void)
{
    uart0_print("\r\n GT9xx touch IC test suite --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m             - show menu\r\n");
    uart0_print(" \e[33;1m!\e[0m             - show status\r\n");
    uart0_print(" \e[33;1mv\e[0m             - set verbose output (default)\r\n");
    uart0_print(" \e[33;1mq\e[0m             - set minimal output\r\n");
    uart0_print(" \e[33;1mt\e[0m             - test function #1\r\n");
    uart0_print(" \e[33;1ms\e[0m             - get status\r\n");
}


void parse_user_input(void)
{
    char itoa_buf[18];

    char *input = uart0_get_rx_buf();
    char f = input[0];

    if (f == '?') {
        display_menu();
    } else if (f == 't') {
        uart0_print("init sent\r\n");
        GT9XX_init(EUSCI_BASE_ADDR, GT9XX_SA);
    } else if (f == 'p') {
        uart0_print("init sent\r\n");
        GT9XX_init(EUSCI_BASE_ADDR, GT9XX_SA);
    } else if (f == 's') {
        uart0_print("read state\r\n");
        //GT9XX_read_state(EUSCI_BASE_ADDR, GT9XX_SA, data);
        //GT9XX_clear_irq(EUSCI_BASE_ADDR, GT9XX_SA);
    } else if (f == '!') {
        uart0_print("stats\r\n sm_count  ");
        uart0_print(_utoh(&itoa_buf[0], timer_a0_get_event()));
        uart0_print("\r\n ta1_get_event  ");
        uart0_print(_utoh(&itoa_buf[0], timer_a1_get_event()));
        uart0_print("\r\n UCA3CTLW0 ");
        uart0_print(_utoh(&itoa_buf[0], UCA3CTLW0));
        uart0_print("\r\n UCA3IE    ");
        uart0_print(_utoh(&itoa_buf[0], UCA3IE));
        uart0_print("\r\n UCA3IFG   ");
        uart0_print(_utoa(&itoa_buf[0], UCA3IFG));
        uart0_print("\r\n TA0CCTL1  ");
        uart0_print(_utoa(&itoa_buf[0], TA0CCTL1));
        uart0_print("\r\n TA0CCR1   ");
        uart0_print(_utoa(&itoa_buf[0], TA0CCR1));
        uart0_print("\r\n TA1CCTL1  ");
        uart0_print(_utoa(&itoa_buf[0], TA1CCTL1));
        uart0_print("\r\n TA1CCTL2  ");
        uart0_print(_utoa(&itoa_buf[0], TA1CCTL2));
        uart0_print("\r\n TA1CCR1   ");
        uart0_print(_utoa(&itoa_buf[0], TA1CCR1));
        uart0_print("\r\n");
    } else if (f == 'v') {
        uart0_print("verbose mode\r\n");
    } else if (f == 'q') {
        uart0_print("quiet mode\r\n");
    } else {
        uart0_print("\r\n");
    }
}

