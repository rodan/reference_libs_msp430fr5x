
/*
    program that replicates an IR signal received via a Vishay TSOP IR receiver

    tweak the uart baud rate in config.h
*/

#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "glue.h"
#include "qa.h"
#include "timer_a0.h"
#include "ir_acquire.h"



void main_init(void)
{
    // port init
    // debug signals (sometimes LEDs)
    P1DIR |= BIT0 + BIT3 + BIT4;

#ifdef USE_XT1
    PJSEL0 |= BIT4 | BIT5;
    CS_setExternalClockSource(32768, 0);
#endif

    // Set DCO Frequency to 8MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);

    // Set DCO Frequency to 1MHz
    //CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_0);

    // configure MCLK, SMCLK to be sourced by DCOCLK
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

#ifdef USE_XT1
    CS_turnOnLFXT(CS_LFXT_DRIVE_3);
#endif
}

static void uart0_rx_irq(uint16_t msg)
{
    parse_user_input();
    uart0_set_eol();
}

static void acquisition_end(uint16_t msg)
{
    qa_acquisition_end();
}

void check_events(void)
{
    struct sys_messagebus *p = sys_messagebus_getp();
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart0_get_event() == UART0_EV_RX) {
        msg |= SYS_MSG_UART0_RX;
        uart0_rst_event();
    }

    // timer
    if (timer_a0_get_event()) {
        msg |= timer_a0_get_event();
        timer_a0_rst_event();
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
    timer_a0_init();

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    sig2_off;

    sys_messagebus_register(&uart0_rx_irq, SYS_MSG_UART0_RX);
    sys_messagebus_register(&acquisition_end, SYS_MSG_TIMER0_CRR2);

    display_menu();

    while (1) {
        // sleep
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        check_events();
    }
}


