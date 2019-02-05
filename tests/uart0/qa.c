
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "qa.h"

#define STR_LEN 64

void display_menu(void)
{
    char str_temp[STR_LEN];

    snprintf(str_temp, STR_LEN,
            "\r\n uart0 and helper module test suite --- available commands:\r\n\r\n");
    uart0_tx_str(str_temp, strlen(str_temp));

    snprintf(str_temp, STR_LEN, " \e[33;1m?\e[0m             - show menu\r\n" );
    uart0_tx_str(str_temp, strlen(str_temp));

    snprintf(str_temp, STR_LEN, " \e[33;1md\e[0m             - convert input into decimal\r\n" );
    uart0_tx_str(str_temp, strlen(str_temp));

    snprintf(str_temp, STR_LEN, " \e[33;1mh\e[0m             - convert input into hex\r\n" );
    uart0_tx_str(str_temp, strlen(str_temp));
}

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
    char f = input[0];
    char str_temp[STR_LEN];

    if (f == '?') {
        display_menu();
    } else if (f == 'd') {

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

