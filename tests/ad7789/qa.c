
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "glue.h"
#include "rtca_now.h"
#include "qa.h"

#define STR_LEN 64

static const char __flash menu[] = "\r\n ad7789 module test suite --- available commands:\r\n\r\n \e[33;1m?\e[0m             - show menu\r\n \e[33;1mi\e[0m             - get reg settings\r\n \e[33;1mc\e[0m             - get conversion\r\n";

void display_menu(void)
{
/*
    uart0_print("\r\n ad7789 module test suite --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m             - show menu\r\n");
    uart0_print(" \e[33;1mi\e[0m             - get reg settings\r\n");
    uart0_print(" \e[33;1mc\e[0m             - get conversion\r\n");
    */
    uart0_print(menu);
}

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
    char f = input[0];
    //char itoa_buf[18];
    char str_temp[STR_LEN];
    uint8_t xfer;
    uint8_t ret;
    uint8_t conv[3];
    float retf;
    int32_t reti;
    //uint32_t in;

    if (f == '?') {
        display_menu();
    } else if (f == 'r') {
        ret = AD7789_rst(EUSCI_SPI_BASE_ADDR);
        snprintf(str_temp, STR_LEN, "ad7789 rst 0x%x\r\n", ret);
        uart0_print(str_temp);

    } else if (f == 'c') {
        ret = AD7789_get_conv(EUSCI_SPI_BASE_ADDR, &conv[0], &retf);
        reti = retf*10000;
        snprintf(str_temp, STR_LEN, "ad7789 ADC %c%d.%04d 0x%x 0x%x 0x%x 0x%x\r\n", reti<0 ? '-':' ',abs(reti/10000), abs(reti%10000), conv[0], conv[1], conv[2], ret);
        uart0_print(str_temp);

    } else if (f == 'i') {
        snprintf(str_temp, STR_LEN, "UCB1CTLW0 0x%x, UCB1BRW 0x%x\r\n", UCB1CTLW0, UCB1BRW);
        uart0_print(str_temp);

        ret = AD7789_get_status(EUSCI_SPI_BASE_ADDR, &xfer);
        snprintf(str_temp, STR_LEN, "ad7789 status 0x%x 0x%x\r\n", xfer, ret);
        uart0_print(str_temp);
    } else {
        uart0_print("\r\n");
    }
}

