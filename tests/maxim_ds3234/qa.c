
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "glue.h"
#include "rtca_now.h"
#include "qa.h"

#define STR_LEN 64

void display_menu(void)
{
    uart0_print("\r\n ds3234 module test suite --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m             - show menu\r\n");
    uart0_print(" \e[33;1mi\e[0m             - get reg settings\r\n");
    uart0_print(" \e[33;1mr\e[0m             - read current time\r\n");
    uart0_print(" \e[33;1mw\e[0m             - write current time\r\n");
}

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
    char f = input[0];
    //char itoa_buf[18];
    char str_temp[STR_LEN];
    //uint32_t in;
    struct ts t;

    if (f == '?') {
        display_menu();
    } else if (f == 'w') {
        t.sec = 0;
        t.min = COMPILE_MIN;
        t.hour = COMPILE_HOUR;
        t.wday = COMPILE_DOW;
        t.mday = COMPILE_DAY;
        t.mon = COMPILE_MON;
        t.year = COMPILE_YEAR;
        DS3234_set(EUSCI_SPI_BASE_ADDR, t);
    } else if (f == 'r') {
        DS3234_get(EUSCI_SPI_BASE_ADDR, &t);

        // there is a compile time option in the library to include unixtime support
#ifdef CONFIG_UNIXTIME
        snprintf(str_temp, STR_LEN, "%d.%02d.%02d %02d:%02d:%02d %ld\r\n", t.year,
                 t.mon, t.mday, t.hour, t.min, t.sec, t.unixtime);
#else
        snprintf(str_temp, STR_LEN, "%d.%02d.%02d %02d:%02d:%02d\r\n", t.year,
                 t.mon, t.mday, t.hour, t.min, t.sec);
#endif
        uart0_print(str_temp);

    } else if (f == 'i') {
        snprintf(str_temp, STR_LEN, "UCB1CTLW0 0x%x, UCB1BRW 0x%x\r\n", UCB1CTLW0, UCB1BRW);
        uart0_print(str_temp);
    } else {
        uart0_print("\r\n");
    }
}

