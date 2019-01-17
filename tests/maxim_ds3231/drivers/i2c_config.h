
// hardware i2c config

/// Select which USCI module to use
#define I2C_USE_DEV     6
/**<    0 = USCIB0 \n
*         1 = USCIB1 \n
*         2 = USCIB2 \n
*         3 = USCIB3
*         4 = EUSCIB0 \n
*         5 = EUSCIB1 \n
*         6 = EUSCIB2 \n
*         7 = EUSCIB3 \n
**/

/// Select which clock source to use
#define I2C_CLK_SRC     2
/**<    0 = External \n
*        1 = ACLK    \n
*        2 = SMCLK
**/

/// Clock Division (0x0004 to 0xFFFF)
#define I2C_CLK_DIV     10

/// optional eUSCI Control Word Register 1
#define I2C_CWR1        0

