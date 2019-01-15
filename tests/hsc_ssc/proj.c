
#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include "proj.h"

#include "drivers/sys_messagebus.h"
#include "drivers/uart0.h"
#include "drivers/timer_a0.h"
#include "drivers/hsc_ssc_i2c.h"

#ifdef HARDWARE_I2C
    #include "drivers/i2c.h"
#endif

// see hsc_ssc_i2c.h for a description of these values
// these defaults are valid for the HSCMRNN030PA2A3 chip
#define SLAVE_ADDR 0x28
#define OUTPUT_MIN 0
#define OUTPUT_MAX 0x3fff       // 2^14 - 1
#define PRESSURE_MIN 0.0        // min is 0 for sensors that give absolute values
#define PRESSURE_MAX 206842.7   // 30psi (and we want results in pascals)

struct cs_raw ps;

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
    char *input;
    uint8_t p;

    input = (char *)uart0_rx_buf;
    p = input[0];

    if (p == 0x67) {       // [g]et value
        ps_get_raw(SLAVE_ADDR, &ps);
        led_switch;
        snprintf(str_temp, STR_LEN, "status: 0x%x\r\n", ps.status);
        uart0_tx_str(str_temp, strlen(str_temp));
        snprintf(str_temp, STR_LEN, "bridge_data: 0x%x\r\n", ps.bridge_data);
        uart0_tx_str(str_temp, strlen(str_temp));
        snprintf(str_temp, STR_LEN, "temperature_data: 0x%x\r\n", ps.temperature_data);
        uart0_tx_str(str_temp, strlen(str_temp));
    } else {
        uart0_tx_str(input, strlen(input));
        snprintf(str_temp, STR_LEN, "char 0x%x\r\n", p);
        uart0_tx_str(str_temp, strlen(str_temp));
        uart0_tx_str("\r\n",2);
    }

    uart0_p = 0;
    uart0_rx_enable = 1;
}

void check_events(void)
{
    struct sys_messagebus *p = messagebus;
    enum sys_message msg = SYS_MSG_NONE;

    // drivers/timer_a0
    if (timer_a0_last_event) {
        msg |= timer_a0_last_event;
        timer_a0_last_event = TIMER_A0_EVENT_NONE;
    }
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

#ifdef HARDWARE_I2C 
    i2c_init();
#endif

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
        //led_switch;
        check_events();
    }

}

