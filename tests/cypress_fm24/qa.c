
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "version.h"
#include "qa.h"

#define STR_LEN 64

void display_memtest(const uint16_t usci_base_addr, const uint8_t slave_addr, const uint32_t start_addr, const uint32_t stop_addr, FM24_test_t test)
{
    uint32_t el;
    uint32_t rows_tested;
    char str_temp[STR_LEN];

    snprintf(str_temp, STR_LEN, " \e[36;1m*\e[0m testing %lx - %lx with pattern #%d\t", start_addr, stop_addr, test);
    uart0_print(str_temp);

    el = FM24_memtest(usci_base_addr, slave_addr, start_addr, stop_addr, test, &rows_tested);

    if (el == 0) { 
        snprintf(str_temp, STR_LEN, "%lu bytes tested \e[32;1mok\e[0m\r\n", rows_tested * 8);
    } else {
        snprintf(str_temp, STR_LEN, "%lu bytes tested with \e[31;1m%lu errors\e[0m\r\n", rows_tested * 8, el );
    }
    uart0_print(str_temp);
}

void display_menu(void)
{
    char str_temp[STR_LEN];

    snprintf(str_temp, STR_LEN,
            "\r\n cypress FM24 test suite v%d.%d --- available commands:\r\n\r\n", COMMIT, BUILD);
    uart0_print(str_temp);
    uart0_print(" \e[33;1m?\e[0m             - show menu\r\n");
    uart0_print(" \e[33;1mi\e[0m             - display i2c registers\r\n");
    uart0_print(" \e[33;1mt\e[0m             - memtest\r\n");
    uart0_print(" \e[33;1mr\e[0m             - tiny read test\r\n");
    uart0_print(" \e[33;1mw\e[0m             - tiny write test\r\n");
    uart0_print(" \e[33;1mh\e[0m             - hex dump read of all FRAM\r\n");
}

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
    char f = input[0];
    uint16_t i;
    uint8_t j;
    uint8_t row[16];
    char str_temp[STR_LEN];

    uint8_t foo[9]="hello wo";
    uint8_t bar[9]="33333333";
    //uint8_t err=0;

    uint8_t data_r[8]; // test 8 bytes (1 row) at a time
    //uint8_t data_w[8];

    //memset(data_w, 0xff, 8);

    if (f == '?') {
        display_menu();
    } else if (f == 'w') {
        FM24_write(EUSCI_BASE_ADDR, FM24_SLAVE_ADDR, foo, FM_LA-20, 8);
    } else if (f == 'b') {
        FM24_write(EUSCI_BASE_ADDR, FM24_SLAVE_ADDR, bar, 0x90, 8);
    } else if (f == 'r') {
        FM24_read(EUSCI_BASE_ADDR, FM24_SLAVE_ADDR, data_r, FM_LA-20, 8);
    } else if (f == 't') {
        //display_memtest(EUSCI_BASE_ADDR, 0xffe0, FM_LA, TEST_00);
        //display_memtest(EUSCI_BASE_ADDR, 0xffe0, FM_LA, TEST_00);
        //display_memtest(EUSCI_BASE_ADDR, 0x10, 0xb0, TEST_AA);
        //display_memtest(EUSCI_BASE_ADDR, 0x40, 0x60, TEST_FF);
        //display_memtest(EUSCI_BASE_ADDR, 0x60, 0x80, TEST_AA);
        //display_memtest(EUSCI_BASE_ADDR, 0x90, 0x98, TEST_00);
        display_memtest(EUSCI_BASE_ADDR, FM24_SLAVE_ADDR, 0, FM_LA, TEST_00);
        display_memtest(EUSCI_BASE_ADDR, FM24_SLAVE_ADDR, 0, FM_LA, TEST_FF);
        display_memtest(EUSCI_BASE_ADDR, FM24_SLAVE_ADDR, 0, FM_LA, TEST_AA);
        uart0_print(" * roll over test\r\n");
        display_memtest(EUSCI_BASE_ADDR, FM24_SLAVE_ADDR, FM_LA - 3, FM_LA + 5, TEST_FF);
    } else if (f == 'h') {
        //for (i=0;i<(FM_LA+1)/16;i++) {
        for (i=0;i<8;i++) {
            FM24_read(EUSCI_BASE_ADDR, FM24_SLAVE_ADDR, row, FM_LA - 63 + (i * 16), 16);
            snprintf(str_temp, STR_LEN, "%08lx: ", FM_LA - 63 + (i * 16));
            uart0_print(str_temp);
            for (j=0; j<8; j++) {
                snprintf(str_temp, STR_LEN, "%02x%02x ", row[2*j], row[2*j+1]);
                uart0_print(str_temp);
            }
            uart0_print("  ");
            for (j=0; j<16; j++) {
                uart0_tx_str((char *)row + j, 1);
            }
            uart0_print("\r\n");
        }

    } else if (f == 'i') {
        snprintf(str_temp, STR_LEN, "P7SEL0 0x%x, P7SEL1 0x%x\r\n", P7SEL0, P7SEL1);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2CTLW0 0x%x, UCB2CTLW1 0x%x\r\n", UCB2CTLW0, UCB2CTLW1);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2BRW 0x%x, UCBxSTATW 0x%x\r\n", UCB2BRW, UCB2STATW);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2TBCNT 0x%x\r\n", UCB2TBCNT);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2RXBUF 0x%x, UCB2TXBUF 0x%x\r\n", UCB2RXBUF, UCB2TXBUF);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2I2COA0 0x%x, UCB2ADDRX 0x%x\r\n", UCB2I2COA0, UCB2ADDRX);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2ADDMASK 0x%x, UCB2I2CSA 0x%x\r\n", UCB2ADDMASK, UCB2I2CSA);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2IE 0x%x, UCB2IFG 0x%x\r\n", UCB2IE, UCB2IFG);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2IV 0x%x\r\n", UCB2IV);
        uart0_print(str_temp);
    } else {
        uart0_print("\r\n");
    }
}

