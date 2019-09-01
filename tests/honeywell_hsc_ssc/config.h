#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "i2c_config.h"

#define USE_XT1
//#define USE_WATCHDOG

// not working with bitbanging i2c  20190901

#define HARDWARE_I2C
#define IRQ_I2C

//#define CONFIG_DEBUG

//#define UART0_SPEED_9600_1M
//#define UART0_SPEED_19200_1M
//#define UART0_SPEED_38400_1M
//#define UART0_SPEED_57600_1M
//#define UART0_SPEED_115200_1M
//#define UART0_SPEED_9600_8M
//#define UART0_SPEED_19200_8M
//#define UART0_SPEED_38400_8M
#define UART0_SPEED_57600_8M
//#define UART0_SPEED_115200_8M
//#define UART0_SPEED_230400_8M
//#define UART0_SPEED_460800_8M

#define CONFIG_HSC_SSC

#endif
