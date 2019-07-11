
/*
    reference MSP430 library for an HI6000 EDGE probe

    tweak the baud rate in config.h
*/

#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "glue.h"
#include "qa.h"
#include "timer_a0.h"
#include "timer_a1.h"

#include "gt9xx.h"

uint8_t coord_buff[100];

struct goodix_ts_data ts;

void main_init(void)
{
    P1OUT = 0;
    P1DIR = 0xFF;

    P2OUT = 0;
    P2DIR = 0xFF;

    P3OUT = 0;
    P3DIR = 0xFF;

    P4OUT = 0;
    P4DIR = 0xFF;

    P5OUT = 0;
    P5DIR = 0xbf;
    P5REN = 0;
    P5IES = 0;
    P5IFG = 0;
    P5IE = 0;
    P5SEL0 = 0;
    P5SEL1 = 0;

    P6OUT = 0;
    P6DIR = 0xff;
    P6REN = 0;
    P6IES = 0;
    P6IFG = 0;
    P6IE = 0;
    P6SEL0 = 0;
    P6SEL1 = 0;

    P7OUT = 0;
    P7DIR = 0xFF;

    P8DIR = 0xFF;
    P8OUT = 0;

    PJOUT = 0;
    PJDIR = 0xFFFF;

    // port init
    P1DIR |= BIT0 + BIT3 + BIT4 + BIT5;

    P5DIR &= ~BIT6;  // set as input
    P5OUT |= BIT6;   // pull-up resistor
    P5REN |= BIT6;   // select pull-up mode
    P5IES |= BIT6;   // IRQ triggers on falling edge
    P5IFG &= ~BIT6;  // reset IRQ flags
//    P5IE |= BIT6;    // enable irq

    P6DIR &= ~GT9XX_IRQ;  // set as input
    P6REN &= ~GT9XX_IRQ;  // disable pullup/pulldown
    // falling edge triggering provides valid data every 15ms cycle
    P6IES |= GT9XX_IRQ;   // IRQ triggers on falling edge
    // rising edge triggering provides valid data every ~30ms
    //P6IES &= ~GT9XX_IRQ;  // IRQ triggers on rising edge
    P6IFG &= ~GT9XX_IRQ;  // reset IRQ flags

#ifdef HARDWARE_I2C
    P7SEL0 |= (BIT0 | BIT1);
    P7SEL1 &= ~(BIT0 | BIT1);
#endif

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

#ifdef HARDWARE_I2C 
    EUSCI_B_I2C_initMasterParam param = {0};

    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = CS_getSMCLK();
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
    param.byteCounterThreshold = 0;
    param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
    EUSCI_B_I2C_initMaster(EUSCI_BASE_ADDR, &param);

    #ifdef IRQ_I2C
        i2c_irq_init(EUSCI_BASE_ADDR);
    #endif
#endif
}

static void uart0_rx_irq(uint16_t msg)
{
    parse_user_input();
    uart0_set_eol();
}

uint8_t data[100];

void check_events(void)
{
    struct sys_messagebus *p = sys_messagebus_getp();
    uint16_t msg = SYS_MSG_NULL;
    uint16_t ev;

    // gt9xx irq
    if (gt9xx_get_event() == GT9XX_EV_IRQ) {
        msg |= SYS_MSG_GT9XX_IRQ;
        gt9xx_rst_event();
    }

    // uart RX
    if (uart0_get_event() == UART0_EV_RX) {
        msg |= SYS_MSG_UART0_RX;
        uart0_rst_event();
    }

    ev = timer_a0_get_event();
    // timer_a0
    if (ev) {
        if ( ev == TIMER_A0_EVENT_CCR1) {
            msg |= SYS_MSG_TIMERA0_CRR1;
        } else if (ev == TIMER_A0_EVENT_CCR2) {
            msg |= SYS_MSG_TIMERA0_CRR2;
        }
        timer_a0_rst_event();
    }

    ev = timer_a1_get_event();
    // timer_a1
    if (ev) {
        if ( ev == TIMER_A1_EVENT_CCR1) {
            msg |= SYS_MSG_TIMERA1_CRR1;
        } else if (ev == TIMER_A1_EVENT_CCR2) {
            msg |= SYS_MSG_TIMERA1_CRR2;
        }
        timer_a1_rst_event();
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
    int16_t rv;
    char itoa_buf[18];

    sig1_on;
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    main_init();
    timer_a0_init(); // edge uart timeout
    timer_a1_init(); // interface - ccr1 - meas interval, ccr2 - blocking delay
    uart0_port_init();
    uart0_init();

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    sig3_on;
    ts.usci_base_addr = EUSCI_BASE_ADDR;
    ts.slave_addr = GT9XX_SA;

    rv = GT9XX_init(&ts);
    sig3_off;

    uart0_print("GT9XX_init ret: ");
    uart0_print(_utoh(&itoa_buf[0], rv));
    uart0_print("\r\n");

    //timer_a1_delay_ccr2(_10ms); // wait 10 ms
    //P5OUT |= BIT2;
    //timer_a1_delay_ccr2(_10ms); // wait 10 ms

    sys_messagebus_register(&uart0_rx_irq, SYS_MSG_UART0_RX);

    display_menu();

    //P5IE |= BIT6;
    GT9XX_enable_irq();
    //P6IE |= GT9XX_IRQ;    // enable irq

    sig1_off;
    sig2_on;
    sig3_on;
    sig4_on;
    sig5_on;

    while (1) {
        // sleep
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        check_events();
        check_events();
        check_events();
        //check_events();
    }
}

/*
// Port 5 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT5_VECTOR
__interrupt void Port_5(void)
#elif defined(__GNUC__)
__attribute__ ((interrupt(PORT5_VECTOR)))
void Port5_ISR(void)
#else
#error Compiler not supported!
#endif
{
    if (P5IFG & BIT6) {
        gt9xx_last_event = 1;
        P5IFG &= ~BIT6;
        LPM3_EXIT;
    }
}
*/
