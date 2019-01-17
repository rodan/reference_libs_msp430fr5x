
#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include "proj.h"
#include "qa.h"

#include "drivers/sys_messagebus.h"
#include "drivers/uart0.h"
#include "drivers/fm24.h"

#ifdef HARDWARE_I2C
    #include "drivers/i2c.h"
#endif
    
#define STR_LEN 64
char str_temp[STR_LEN];

void main_init(void)
{
    // port init
    P1DIR = BIT0;

#ifdef HARDWARE_I2C
    P7SEL0 |= (BIT0 | BIT1);
    P7SEL1 &= ~(BIT0 | BIT1);
#endif

    // XT1
    PJSEL0 |= BIT4 | BIT5;                  // For XT1
    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers
    CSCTL4 &= ~LFXTOFF;
    do
    {
        CSCTL5 &= ~LFXTOFFG;                // Clear XT1 fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);              // Test oscillator fault flag
    CSCTL0_H = 0;                           // Lock CS registers
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

#ifdef HARDWARE_I2C 
    i2c_init();
#endif

    led_off;

    sys_messagebus_register(&uart0_rx_irq, SYS_MSG_UART0_RX);

    while (1) {
        // sleep
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        //led_switch;
        check_events();
    }

}

