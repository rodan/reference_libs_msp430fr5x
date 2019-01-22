
#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include "proj.h"

#include "driverlib.h"
#include "sys_messagebus.h"
#include "uart0.h"
#include "qa.h"

void main_init(void)
{
    // port init
    P1DIR = BIT0;

#ifdef USE_XT1
    PJSEL0 |= BIT4 | BIT5;
    CS_setExternalClockSource(32768,0);
#endif

    // Set DCO Frequency to 8MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);

    // configure MCLK, SMCLK to be source by DCOCLK
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

#ifdef USE_XT1
    CS_turnOnLFXT(CS_LFXT_DRIVE_3);
#endif

}

static void uart0_rx_irq(enum sys_message msg)
{
    parse_user_input();

    uart0_p = 0;
    uart0_rx_enable = 1;
}

void check_events(void)
{
    struct sys_messagebus *p = messagebus;
    enum sys_message msg = SYS_MSG_NONE;

    // uart RX
    if (uart0_last_event == UART0_EV_RX) {
        msg |= SYS_MSG_UART0_RX;
        uart0_last_event = UART0_EV_NONE;
    }

    while (p) {
        // notify listener if he registered for any of these messages
        if (msg & p->listens) {
            p->fn(msg);
        }
        p = p->next;
    }
}

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    main_init();
    uart0_port_init();
    uart0_init();

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    sys_messagebus_register(&uart0_rx_irq, SYS_MSG_UART0_RX);

    while (1) {
        // sleep
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        led_switch;
        check_events();
    }

}
