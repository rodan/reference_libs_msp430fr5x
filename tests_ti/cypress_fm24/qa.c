
#include <stdio.h>
#include <string.h>

#include "fm24_memtest.h"
#include "uart0.h"
#include "fm24.h"
#include "i2c.h"
#include "qa.h"

void display_memtest(const uint16_t usci_base_addr, const uint32_t start_addr, const uint32_t stop_addr, fm24_test_t test)
{
    uint32_t el;
    uint32_t rows_tested;

    snprintf(str_temp, STR_LEN, " \e[36;1m*\e[0m testing %lx - %lx with pattern #%d\t", start_addr, stop_addr, test);
    uart0_tx_str(str_temp, strlen(str_temp));

    el = fm24_memtest(usci_base_addr, start_addr, stop_addr, test, &rows_tested);

    if (el == 0) { 
        snprintf(str_temp, STR_LEN, "%lu bytes tested \e[32;1mok\e[0m\r\n", rows_tested * 8);
    } else {
        snprintf(str_temp, STR_LEN, "%lu bytes tested with \e[31;1m%lu errors\e[0m\r\n", rows_tested * 8, el );
    }
    uart0_tx_str(str_temp, strlen(str_temp));
}

void display_menu(void)
{
    snprintf(str_temp, STR_LEN,
            "\r\n cypress FM24 test suite --- available commands:\r\n\r\n");
    uart0_tx_str(str_temp, strlen(str_temp));

    snprintf(str_temp, STR_LEN, " \e[33;1m?\e[0m             - show menu\r\n" );
    uart0_tx_str(str_temp, strlen(str_temp));

    snprintf(str_temp, STR_LEN, " \e[33;1mi\e[0m             - display i2c registers\r\n" );
    uart0_tx_str(str_temp, strlen(str_temp));

    snprintf(str_temp, STR_LEN, " \e[33;1mmem test\e[0m      - memtest\r\n" );
    uart0_tx_str(str_temp, strlen(str_temp));

    snprintf(str_temp, STR_LEN, " \e[33;1mmem raw\e[0m       - raw read of all FRAM\r\n" );
    uart0_tx_str(str_temp, strlen(str_temp));

    snprintf(str_temp, STR_LEN, " \e[33;1mmem hex\e[0m       - hex dump read of all FRAM\r\n" );
    uart0_tx_str(str_temp, strlen(str_temp));

}

void parse_user_input(void)
{
    char f = uart0_rx_buf[0];
    uint16_t i;
    uint8_t j;
    uint8_t row[16];

    uint8_t foo[9]="hello wo";
    uint8_t bar[9]="33333333";
    uint8_t err=0;

    uint8_t data_r[8]; // test 8 bytes (1 row) at a time
    //uint8_t data_w[8];

    //memset(data_w, 0xff, 8);

    if (f == '?') {
        display_menu();
    } else if (f == 'w') {
        fm24_write(EUSCI_BASE_ADDR, foo, FM_LA-20, 8);
    } else if (f == 'b') {
        fm24_write(EUSCI_BASE_ADDR, bar, 0x90, 8);
    } else if (f == 'r') {
        fm24_read(EUSCI_BASE_ADDR, data_r, FM_LA-20, 8);
    } else if (f == 't') {
        //display_memtest(EUSCI_BASE_ADDR, 0xffe0, FM_LA, TEST_00);
        //display_memtest(EUSCI_BASE_ADDR, 0xffe0, FM_LA, TEST_00);
        //display_memtest(EUSCI_BASE_ADDR, 0x10, 0xb0, TEST_AA);
        //display_memtest(EUSCI_BASE_ADDR, 0x40, 0x60, TEST_FF);
        //display_memtest(EUSCI_BASE_ADDR, 0x60, 0x80, TEST_AA);
        //display_memtest(EUSCI_BASE_ADDR, 0x90, 0x98, TEST_00);
        display_memtest(EUSCI_BASE_ADDR, 0, FM_LA, TEST_00);
        display_memtest(EUSCI_BASE_ADDR, 0, FM_LA, TEST_FF);
        display_memtest(EUSCI_BASE_ADDR, 0, FM_LA, TEST_AA);
        uart0_tx_str(" * roll over test\r\n", 19);
        display_memtest(EUSCI_BASE_ADDR, FM_LA - 3, FM_LA + 5, TEST_FF);
    } else if (f == 'h') {
        //for (i=0;i<(FM_LA+1)/16;i++) {
        for (i=0;i<8;i++) {
            fm24_read(EUSCI_BASE_ADDR, row, FM_LA - 63 + (i * 16), 16);
            snprintf(str_temp, STR_LEN, "%08lx: ", FM_LA - 63 + (i * 16));
            uart0_tx_str(str_temp, strlen(str_temp));
            for (j=0; j<8; j++) {
                snprintf(str_temp, STR_LEN, "%02x%02x ", row[2*j], row[2*j+1]);
                uart0_tx_str(str_temp, strlen(str_temp));
            }
            uart0_tx_str("  ", 2);
            for (j=0; j<16; j++) {
                uart0_tx_str((char *)row + j, 1);
            }
            uart0_tx_str("\r\n", 2);
        }

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

    }
}

