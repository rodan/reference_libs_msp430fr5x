
#include <stdio.h>
#include <string.h>

#include "uart0.h"
#include "ds3231.h"
#include "i2c.h"
#include "rtca_now.h"
#include "qa.h"
//#include "i2c_config.h"

#define STR_LEN 64

void display_menu(void)
{
    char str_temp[STR_LEN];

    snprintf(str_temp, STR_LEN, "\r\n Maxim DS3231 test suite --- available commands:\r\n\r\n");
    uart0_tx_str(str_temp, strlen(str_temp));

    snprintf(str_temp, STR_LEN, " \e[33;1m?\e[0m             - show menu\r\n");
    uart0_tx_str(str_temp, strlen(str_temp));

    snprintf(str_temp, STR_LEN, " \e[33;1mi\e[0m             - display i2c registers\r\n");
    uart0_tx_str(str_temp, strlen(str_temp));

    snprintf(str_temp, STR_LEN, " \e[33;1mr\e[0m             - read current time\r\n");
    uart0_tx_str(str_temp, strlen(str_temp));

    snprintf(str_temp, STR_LEN, " \e[33;1mw\e[0m             - write current time\r\n");
    uart0_tx_str(str_temp, strlen(str_temp));

    snprintf(str_temp, STR_LEN, " \e[33;1mt\e[0m             - read temperature\r\n");
    uart0_tx_str(str_temp, strlen(str_temp));
}

void parse_user_input(void)
{
    char f = uart0_rx_buf[0];
    struct ts t;
    char str_temp[STR_LEN];

    if (f == '?') {
        display_menu();
    } else if (f == 'r') {

        DS3231_get(EUSCI_BASE_ADDR, &t);

        // there is a compile time option in the library to include unixtime support
#ifdef CONFIG_UNIXTIME
        snprintf(str_temp, STR_LEN, "%d.%02d.%02d %02d:%02d:%02d %ld\r\n", t.year,
                 t.mon, t.mday, t.hour, t.min, t.sec, t.unixtime);
#else
        snprintf(str_temp, STR_LEN, "%d.%02d.%02d %02d:%02d:%02d\r\n", t.year,
                 t.mon, t.mday, t.hour, t.min, t.sec);
#endif
        uart0_tx_str(str_temp, strlen(str_temp));
    } else if (f == 'w') {
        t.sec = 0;
        t.min = COMPILE_MIN;
        t.hour = COMPILE_HOUR;
        t.wday = COMPILE_DOW;
        t.mday = COMPILE_DAY;
        t.mon = COMPILE_MON;
        t.year = COMPILE_YEAR;
        DS3231_set(EUSCI_BASE_ADDR, t);
    } else if (f == 't') {
        //DS3231_get_treg();
    } else if (f == 'i') {
        snprintf(str_temp, STR_LEN, "P7SEL0 0x%x, P7SEL1 0x%x\r\n", P7SEL0, P7SEL1);
        uart0_tx_str(str_temp, strlen(str_temp));

        snprintf(str_temp, STR_LEN, "UCB2CTLW0 0x%x, UCB2CTLW1 0x%x\r\n", UCB2CTLW0, UCB2CTLW1);
        uart0_tx_str(str_temp, strlen(str_temp));

        snprintf(str_temp, STR_LEN, "UCB2BRW 0x%x, UCBxSTATW 0x%x\r\n", UCB2BRW, UCB2STATW);
        uart0_tx_str(str_temp, strlen(str_temp));

        snprintf(str_temp, STR_LEN, "UCB2TBCNT 0x%x\r\n", UCB2TBCNT);
        uart0_tx_str(str_temp, strlen(str_temp));

        snprintf(str_temp, STR_LEN, "UCB2RXBUF 0x%x, UCB2TXBUF 0x%x\r\n", UCB2RXBUF, UCB2TXBUF);
        uart0_tx_str(str_temp, strlen(str_temp));

        snprintf(str_temp, STR_LEN, "UCB2I2COA0 0x%x, UCB2ADDRX 0x%x\r\n", UCB2I2COA0, UCB2ADDRX);
        uart0_tx_str(str_temp, strlen(str_temp));

        snprintf(str_temp, STR_LEN, "UCB2ADDMASK 0x%x, UCB2I2CSA 0x%x\r\n", UCB2ADDMASK, UCB2I2CSA);
        uart0_tx_str(str_temp, strlen(str_temp));

        snprintf(str_temp, STR_LEN, "UCB2IE 0x%x, UCB2IFG 0x%x\r\n", UCB2IE, UCB2IFG);
        uart0_tx_str(str_temp, strlen(str_temp));

        snprintf(str_temp, STR_LEN, "UCB2IV 0x%x\r\n", UCB2IV);
        uart0_tx_str(str_temp, strlen(str_temp));
    } else {
        uart0_tx_str("\r\n", 2);
    }
}
