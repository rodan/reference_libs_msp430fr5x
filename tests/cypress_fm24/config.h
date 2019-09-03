#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "i2c_config.h"

#define USE_XT1
//#define USE_WATCHDOG
#define HARDWARE_I2C
//#define IRQ_I2C

#define CONFIG_CYPRESS_FM24
//#define CONFIG_FM24CL64B
#define CONFIG_FM24V10

// only a few of the F-RAM chips have explicit sleep mode option
//#ifdef CONFIG_FM24V10
//    #define FM24_HAS_SLEEP_MODE
//#endif

//#define CONFIG_DEBUG

#define UART0_SPEED_57600_8M

#endif
