#ifndef __PROJ_H__
#define __PROJ_H__

#include <msp430.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"

#define led_on              P1OUT |= BIT0
#define led_off             P1OUT &= ~BIT0
#define led_switch          P1OUT ^= BIT0

#define true                1
#define false               0

/*!
	\brief List of possible message types for the message bus.
	\sa sys_messagebus_register()
*/

#define           SYS_MSG_NULL 0
#define    SYS_MSG_TIMER0_CRR1 0x1   // timer_a0_delay_noblk_ccr1
#define    SYS_MSG_TIMER0_CRR2 0x2   // timer_a0_delay_noblk_ccr2
#define     SYS_MSG_TIMER0_IFG 0x4   // timer0 overflow
#define       SYS_MSG_UART0_RX 0x8   // UART received something

// see hsc_ssc.h for a description of these values
// these defaults are valid for the HSCMRNN030PA2A3 chip
#define SLAVE_ADDR 0x28
#define OUTPUT_MIN 0
#define OUTPUT_MAX 0x3fff       // 2^14 - 1
#define PRESSURE_MIN 0.0        // min is 0 for sensors that give absolute values
#define PRESSURE_MAX 206842.7   // 30psi (and we want results in pascals)


#endif
