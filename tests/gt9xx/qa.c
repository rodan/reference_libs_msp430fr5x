
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "qa.h"
#include "timer_a0.h"
#include "timer_a1.h"
#include "gt9xx.h"

extern struct goodix_ts_data ts;
extern uint8_t gt9xx_conf[GT9XX_CONFIG_911_SZ];
extern uint8_t rescale;

void display_menu(void)
{
    uart0_print("\r\n GT9xx touch IC test suite --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m       - show menu\r\n");
    //uart0_print(" \e[33;1m!\e[0m             - show status\r\n");
    //uart0_print(" \e[33;1mv\e[0m             - set verbose output (default)\r\n");
    //uart0_print(" \e[33;1mq\e[0m             - set minimal output\r\n");
    uart0_print(" \e[33;1mrrtc\e[0m    - read real-time cmd register\r\n");
    uart0_print(" \e[33;1mrcs\e[0m     - read cmd status register\r\n");
    uart0_print(" \e[33;1mcr\e[0m      - config read\r\n");
    uart0_print(" \e[33;1mct\e[0m      - config test\r\n");
    uart0_print(" \e[33;1mcd\e[0m      - config default\r\n");
    uart0_print(" \e[33;1mr[01]\e[0m   - rescale on/off\r\n");
}

void parse_user_input(void)
{
    char itoa_buf[18];

    char *input = uart0_get_rx_buf();
    char f = input[0];
    uint8_t temp_buff[255];
    int16_t rv;
    uint16_t i,j;

    temp_buff[0] = 0xff;

    if (f == '?') {
        display_menu();
    } else if (strstr (input, "rcs")) {
        GT9XX_read(&ts, GT9XX_rCMD_STATUS, temp_buff, 1);
        uart0_print(_utoh(&itoa_buf[0], temp_buff[0]));
        uart0_print("\r\n");
    } else if (strstr (input, "rrtc")) {
        GT9XX_read(&ts, GT9XX_rCMD, temp_buff, 1);
        uart0_print(_utoh(&itoa_buf[0], temp_buff[0]));
        uart0_print("\r\n");
    } else if (strstr (input, "cr")) {
        GT9XX_free_config(&ts);
        rv = GT9XX_read_config(&ts);
        if (rv) {
            uart0_print("[!!] read conf\r\n");
            return;
        }
        //uart0_print("[ok] read conf\r\n");

        ts.conf.data[rOFF_FRESH_CONFIG] = 1;
        rv = GT9xx_check_conf_8(&ts.conf);
        if (rv) {
            uart0_print("[!!] check conf\r\n");
            return;
        }
        uart0_print("[ok] check conf ");
        uart0_print(_utoh(&itoa_buf[0], ts.conf.data[rOFF_MAX_CONTACTS]));

        uart0_print("\r\n");
        j=0;
        for (i=0; i<GT9XX_CONFIG_911_SZ; i++) {
            if (j%10 == 0) {
                uart0_print("\r\n");
            }
            uart0_print(_utoh(&itoa_buf[0], ts.conf.data[i]));
            uart0_print(", ");
            j++;
        }
        uart0_print("\r\n");

    } else if (strstr (input, "cd")) {
        rv = GT9XX_write_config(&ts, (uint8_t *) &gt9xx_conf, GT9XX_CONFIG_911_SZ);
        if (rv) {
            uart0_print("[!!] write conf\r\n");
        } else {
            uart0_print("[ok] write conf\r\n");
        }
    } else if (strstr (input, "ct")) {
        GT9XX_free_config(&ts);

        rv = GT9XX_read_config(&ts);
        if (rv) {
            uart0_print("[!!] read conf\r\n");
            return;
        }
        //uart0_print("[ok] read conf\r\n");

        ts.conf.data[rOFF_FRESH_CONFIG] = 1;
        rv = GT9xx_check_conf_8(&ts.conf);
        if (rv) {
            uart0_print("[!!] check conf\r\n");
            return;
        }
        //uart0_print("[ok] check conf\r\n");

        //uart0_print(_utoh(&itoa_buf[0], ts.conf.data[rOFF_MAX_CONTACTS]));
        ts.conf.data[rOFF_CONF_VER] = 0x42;
        ts.conf.data[rOFF_MAX_CONTACTS] = 4;
        ts.conf.data[rOFF_CHECKSUM] = GT9XX_calc_checksum(ts.conf.data, GT9XX_CONFIG_911_SZ - 2);
        ts.conf.data[rOFF_FRESH_CONFIG] = 1;
        //uart0_print(_utoh(&itoa_buf[0], ts.conf.data[rOFF_CHECKSUM]));
        rv = GT9xx_check_conf_8(&ts.conf);
        if (rv) {
            uart0_print("[!!] check conf\r\n");
            return;
        }
        //uart0_print("[ok] check conf\r\n");

        rv = GT9XX_write_config(&ts, ts.conf.data, GT9XX_CONFIG_911_SZ);
        if (rv) {
            uart0_print("[!!] write conf\r\n");
            return;
        }
        uart0_print("[ok] write conf\r\n");

        uart0_print("\r\n");
    } else if (strstr (input, "r0")) {
        rescale = 0;
    } else if (strstr (input, "r1")) {
        rescale = 1;
/*
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
        */
    } else {
        uart0_print("\r\n");
    }
}

