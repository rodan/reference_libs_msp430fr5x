
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "glue.h"
#include "ui.h"

#define STR_LEN 64

void display_menu(void)
{
    uart0_print("\r\n uart0 and helper module test suite --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m             - show menu\r\n");
    uart0_print(" \e[33;1md\e[0m             - convert input into decimal\r\n" );
    uart0_print(" \e[33;1mh\e[0m             - convert input into hex\r\n" );
}

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
    char f = input[0];
    char itoa_buf[18];
    uint32_t in=0;
    int32_t si=0;

    if (f == '?') {
        display_menu();
    } else if (f == 'd') {
        if (str_to_int32(input, &si, 1, strlen(input) - 1, -2147483648, 2147483647) == EXIT_FAILURE) {
            uart0_print("error during str_to_int32()");
        }
        uart0_print("received ");
        uart0_print(_itoa(itoa_buf, si));
        uart0_print("\r\n");
    } else if (f == 'h') {
        if (str_to_uint32(input, &in, 1, strlen(input) - 1, 0, -1) == EXIT_FAILURE) {
            uart0_print("error during str_to_uint32()");
        }
        uart0_print("received ");
        uart0_print(_utoh(itoa_buf, in));
        uart0_print("\r\n");
    } else if (f == 'a') {
        //uart0_tx_str("123456789\r\n", 11);
        uart0_print("123456789\r\n");
    } else {
        //uart0_tx_str("\r\n", 2);
    }
}
