#ifndef __GLUE_H__
#define __GLUE_H__

#include "uart0.h"
#include "helper.h"
#include "sys_messagebus.h"

#include "spi.h"
#include "ds3234.h"

#ifdef __I2C_CONFIG_H__
#include "i2c.h"
#include "serial_bitbang.h"

#include "ds3231.h"
#include "fm24.h"
#include "fm24_memtest.h"
#include "hsc_ssc.h"
#include "sht1x.h"
#endif

#endif
