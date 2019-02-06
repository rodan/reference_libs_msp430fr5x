#ifndef __PROJ_H__
#define __PROJ_H__

#include <msp430.h>
#include "config.h"

#define led_on              P1OUT |= BIT0
#define led_off             P1OUT &= ~BIT0
#define led_switch          P1OUT ^= BIT0

#endif
