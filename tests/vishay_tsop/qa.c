
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "qa.h"
#include "ir_acquire.h"

#define STR_LEN 64

void display_menu(void)
{
    uart0_print("\r\n vishay TSOP aquisition module test suite --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m             - show menu\r\n");
    uart0_print(" \e[33;1ms\e[0m             - start IR aquisition\r\n" );
    uart0_print(" \e[33;1md\e[0m             - convert input into decimal\r\n" );
    
}

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
    char f = input[0];
    char itoa_buf[18];
    uint32_t in;

    if (f == '?') {
        display_menu();
    } else if (f == 'd') {
        if (str_to_uint32(input, &in, 1, strlen(input) - 1, 0, -1) == EXIT_FAILURE) {
            uart0_print("error during str_to_uint32()");
        }
        uart0_print("received ");
        uart0_print(_utoa(itoa_buf, in));
        uart0_print("\r\n");
    } else if (f == 'i') {
        uart0_print("P1IN ");
        uart0_print(_utob(itoa_buf, P1IN));
        uart0_print("\r\n P1IE");
        uart0_print(_utob(itoa_buf, P1IE));
        uart0_print("\r\n P1IES");
        uart0_print(_utob(itoa_buf, P1IES));
        uart0_print("\r\n P1IFG");
        uart0_print(_utob(itoa_buf, P1IFG));
        uart0_print("\r\n");
    } else if (f == 's') {
        ir_acquire_start();
        uart0_print("go\r\n");
    } else {
        uart0_tx_str("\r\n", 2);
    }
}

