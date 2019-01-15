#ifndef __PROJ_H__
#define __PROJ_H__

#include <msp430.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"

// bitbang i2c
#define I2C_MASTER_DIR      P7DIR
#define I2C_MASTER_OUT      P7OUT
#define I2C_MASTER_IN       P7IN
#define I2C_MASTER_SCL      BIT1
#define I2C_MASTER_SDA      BIT0

#define led_on              P1OUT |= BIT0
#define led_off             P1OUT &= ~BIT0
#define led_switch          P1OUT ^= BIT0

#define true                1
#define false               0

#endif
