#ifndef _CONFIG_H_
#define _CONFIG_H_

#define USE_XT1
//#define USE_WATCHDOG
#define HARDWARE_I2C
#define IRQ_I2C

// eUSCI
#define EUSCI_BASE_ADDR     EUSCI_B2_BASE
#define I2C_ISR_VECTOR      EUSCI_B2_VECTOR

#define I2C_CTL0        UCB2CTLW0
#define I2C_CTL1        UCB2CTLW0
#define I2C_CTLW1       UCB2CTLW1
#define I2C_BR          UCB2BRW
#define I2C_STAT        UCB2STATW
#define I2C_RXBUF       UCB2RXBUF
#define I2C_TXBUF       UCB2TXBUF
#define I2C_OA          UCB2I2COA0
#define I2C_SA          UCB2I2CSA
#define I2C_IE          UCB2IE
#define I2C_IFG         UCB2IFG
#define I2C_IV          UCB2IV
#define I2C_ISR_VECTOR  EUSCI_B2_VECTOR

#endif
