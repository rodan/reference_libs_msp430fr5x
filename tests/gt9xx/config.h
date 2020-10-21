#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "i2c_config.h"

#define USE_XT1
//#define USE_WATCHDOG
#define HARDWARE_I2C
#define IRQ_I2C

#define  GT9XX_SA 0x5d
//#define  GT9XX_SA 0x14
#define GT9XX_IRQ BIT3 // port for the irq functionality
#define GT9XX_RST BIT2 // port for the reset functionality
#define GT9XX_CONF_VER_C

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

//#define USE_ITOA_LUT
//#define CONFIG_DEBUG

// read 50bytes of status registers after every interrupt
//#define DEBUG_STATUS

// if enabled automatically calculate the checksum for the written firmware
// otherwise abort writing if checksum is incorrect
#define OVERWRITE_CHECKSUM

#endif
