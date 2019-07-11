
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "qa.h"
#include "timer_a0.h"
#include "timer_a1.h"
#include "gt9xx.h"

void display_menu(void)
{
    uart0_print("\r\n GT9xx touch IC test suite --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m             - show menu\r\n");
    uart0_print(" \e[33;1m!\e[0m             - show status\r\n");
    uart0_print(" \e[33;1mv\e[0m             - set verbose output (default)\r\n");
    uart0_print(" \e[33;1mq\e[0m             - set minimal output\r\n");
    uart0_print(" \e[33;1mt\e[0m             - test function #1\r\n");
    uart0_print(" \e[33;1ms\e[0m             - get status\r\n");
}

extern struct goodix_ts_data ts;

void parse_user_input(void)
{
    char itoa_buf[18];

    char *input = uart0_get_rx_buf();
    char f = input[0];
    uint8_t temp_buff[255];
    int16_t rv;

    temp_buff[0] = 0xff;

    if (f == '?') {
        display_menu();
    } else if (strstr (input, "rcs")) {
        uart0_print("read command status\r\n");
        GT9XX_read(&ts, GT9XX_rCMD_STATUS, temp_buff, 1);
        uart0_print(_utoh(&itoa_buf[0], temp_buff[0]));
        uart0_print("\r\n");
    } else if (strstr (input, "rrtc")) {
        uart0_print("read real-time command register\r\n");
        GT9XX_read(&ts, GT9XX_rCMD, temp_buff, 1);
        uart0_print(_utoh(&itoa_buf[0], temp_buff[0]));
        uart0_print("\r\n");
    } else if (strstr (input, "rconf")) {
        uart0_print("read 0x8047-0x8100\r\n");

        rv = GT9XX_read_config(&ts);
        if (rv) {
            uart0_print("[!!] read conf\r\n");
            return;
        }
        uart0_print("[ok] read conf\r\n");

        ts.conf.data[rOFF_FRESH_CONFIG] = 1;
        rv = GT9xx_check_cfg_8(&ts, &ts.conf);
        if (rv) {
            uart0_print("[!!] check conf\r\n");
            return;
        }
        uart0_print("[ok] check conf\r\n");
        uart0_print(_utoh(&itoa_buf[0], ts.conf.data[rOFF_MAX_CONTACTS]));
    } else if (strstr (input, "tconf")) {
        uart0_print("read/write 0x8047-0x8100\r\n");

        rv = GT9XX_read_config(&ts);
        if (rv) {
            uart0_print("[!!] read conf\r\n");
            return;
        }
        uart0_print("[ok] read conf\r\n");

        ts.conf.data[rOFF_FRESH_CONFIG] = 1;
        rv = GT9xx_check_cfg_8(&ts, &ts.conf);
        if (rv) {
            uart0_print("[!!] check conf\r\n");
            return;
        }
        uart0_print("[ok] check conf\r\n");

        uart0_print(_utoh(&itoa_buf[0], ts.conf.data[rOFF_MAX_CONTACTS]));
        ts.conf.data[rOFF_MAX_CONTACTS] = 2;
        ts.conf.data[rOFF_CHECKSUM] = GT9XX_calc_checksum(ts.conf.data, GT9XX_CONFIG_911_SZ - 2);
        ts.conf.data[rOFF_FRESH_CONFIG] = 1;
        uart0_print(_utoh(&itoa_buf[0], ts.conf.data[rOFF_CHECKSUM]));
        rv = GT9xx_check_cfg_8(&ts, &ts.conf);
        if (rv) {
            uart0_print("[!!] check conf\r\n");
            return;
        }
        uart0_print("[ok] check conf\r\n");

        rv = GT9XX_write_config(&ts);
        if (rv) {
            uart0_print("[!!] write conf\r\n");
            return;
        }
        uart0_print("[ok] write conf\r\n");

        uart0_print("\r\n");
    } else if (f == 's') {
        uart0_print("read state\r\n");
        //GT9XX_read_state(EUSCI_BASE_ADDR, GT9XX_SA, data);
        //GT9XX_clear_irq(EUSCI_BASE_ADDR, GT9XX_SA);
    } else if (f == '!') {
        uart0_print("stats\r\n sm_count  ");
        uart0_print(_utoh(&itoa_buf[0], timer_a0_get_event()));
        uart0_print("\r\n ta1_get_event  ");
        uart0_print(_utoh(&itoa_buf[0], timer_a1_get_event()));
        uart0_print("\r\n UCA3CTLW0 ");
        uart0_print(_utoh(&itoa_buf[0], UCA3CTLW0));
        uart0_print("\r\n UCA3IE    ");
        uart0_print(_utoh(&itoa_buf[0], UCA3IE));
        uart0_print("\r\n UCA3IFG   ");
        uart0_print(_utoa(&itoa_buf[0], UCA3IFG));
        uart0_print("\r\n TA0CCTL1  ");
        uart0_print(_utoa(&itoa_buf[0], TA0CCTL1));
        uart0_print("\r\n TA0CCR1   ");
        uart0_print(_utoa(&itoa_buf[0], TA0CCR1));
        uart0_print("\r\n TA1CCTL1  ");
        uart0_print(_utoa(&itoa_buf[0], TA1CCTL1));
        uart0_print("\r\n TA1CCTL2  ");
        uart0_print(_utoa(&itoa_buf[0], TA1CCTL2));
        uart0_print("\r\n TA1CCR1   ");
        uart0_print(_utoa(&itoa_buf[0], TA1CCR1));
        uart0_print("\r\n");
    } else if (f == 'v') {
        uart0_print("verbose mode\r\n");
    } else if (f == 'q') {
        uart0_print("quiet mode\r\n");
    } else {
        uart0_print("\r\n");
    }
}

