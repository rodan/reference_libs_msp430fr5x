
#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "glue.h"
#include "qa.h"

void main_init(void)
{
    // port init
    P1DIR = BIT0;

#ifdef USE_XT1
    PJSEL0 |= BIT4 | BIT5;
    CS_setExternalClockSource(32768, 0);
#endif

    // Set DCO Frequency to 8MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);

    // configure MCLK, SMCLK to be source by DCOCLK
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

#ifdef USE_XT1
    CS_turnOnLFXT(CS_LFXT_DRIVE_3);
#endif
}

static void uart0_rx_irq(uint16_t msg)
{
    //parse_user_input();
    uart0_set_eol();
}

void check_events(void)
{
    struct sys_messagebus *p = sys_messagebus_getp();
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart0_get_event() == UART0_EV_RX) {
        msg |= SYS_MSG_UART0_RX;
        uart0_rst_event();
    }

    while (p) {
        // notify listener if he registered for any of these messages
        if (msg & p->listens) {
            p->fn(msg);
        }
        p = p->next;
    }
}

int main(void)
{
    char buf[64];

    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    main_init();
    uart0_port_init();
    uart0_init();

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    led_off;

    sys_messagebus_register(&uart0_rx_irq, SYS_MSG_UART0_RX);

//#define TEST_UART0_TX_STR
//#define TEST_UART0_PRINT
//#define TEST_ITOA
//#define TEST_SNPRINTF
//#define TEST_UTOH
//#define TEST_UTOB

#ifdef TEST_UART0_TX_STR
    uart0_tx_str("h1llo world\r\n",13);
    uart0_tx_str("he2lo world\r\n",13);
    uart0_tx_str("hel3o world\r\n",13);
    uart0_tx_str("hell4 world\r\n",13);
    uart0_tx_str("hello5world\r\n",13);
    uart0_tx_str("hello 6orld\r\n",13);
    uart0_tx_str("hello w7rld\r\n",13);
    uart0_tx_str("hello wo8ld\r\n",13);
    uart0_tx_str("hello wor9d\r\n",13);
    uart0_tx_str("hello worl0\r\n",13);
#endif

#ifdef TEST_UART0_PRINT
    uart0_print("h1llo world\r\n");
    uart0_print("he2lo world\r\n");
    uart0_print("hel3o world\r\n");
    uart0_print("hell4 world\r\n");
    uart0_print("hello5world\r\n");
    uart0_print("hello 6orld\r\n");
    uart0_print("hello w7rld\r\n");
    uart0_print("hello wo8ld\r\n");
    uart0_print("hello wor9d\r\n");
    uart0_print("hello worl0\r\n");
#endif

#ifdef TEST_ITOA
    uart0_print(_itoa(&buf[0], 0));
    uart0_print("\r\n");
    uart0_print(_itoa(&buf[0], 65535));
    uart0_print("\r\n");
    uart0_print(_utoa(&buf[0], 4294967295));
    uart0_print("\r\n");
    uart0_print(_itoa(&buf[0], -65535));
    uart0_print("\r\n");
    uart0_print(_itoa(&buf[0], -1));
    uart0_print("\r\n");
//>> Building main.elf as target RELEASE
//   text    data     bss     dec     hex filename
//   2627     212     106    2945     b81 main.elf
//with BIG_ITOA_TABLE
//   2835     212     106    3153     c51 main.elf
#endif

#ifdef TEST_SNPRINTF
    snprintf(buf, 12, "%d", 0);
    uart0_tx_str(buf, strlen(buf));
    snprintf(buf, 12, "%ld", 65535);
    uart0_tx_str(buf, strlen(buf));
    snprintf(buf, 12, "%lld", 4294967295);
    uart0_tx_str(buf, strlen(buf));
    snprintf(buf, 12, "%ld", -65535);
    uart0_tx_str(buf, strlen(buf));
    snprintf(buf, 12, "%d", -1);
    uart0_tx_str(buf, strlen(buf));
//>> Building main.elf as target RELEASE
//   text    data     bss     dec     hex filename
//   5151     212     106    5469    155d main.elf
#endif

#ifdef TEST_UTOH
    uart0_print(_utoh(&buf[0], 0));
    uart0_print("\r\n");
    uart0_print(_utoh(&buf[0], 0x0000ffff));
    uart0_print("\r\n");
    uart0_print(_utoh(&buf[0], 0xffffffff));
    uart0_print("\r\n");
    uart0_print(_utoh(&buf[0], 0x1010cafe));
    uart0_print("\r\n");
    uart0_print(_utoh(&buf[0], 0xa1b2c3e4));
    uart0_print("\r\n");
#endif

#ifdef TEST_UTOB
    uart0_print(_utob(&buf[0], 0));
    uart0_print("\r\n");
    uart0_print(_utob(&buf[0], 0x0000ffff));
    uart0_print("\r\n");
    uart0_print(_utob(&buf[0], 0xffffffff));
    uart0_print("\r\n");
    uart0_print(_utob(&buf[0], 0x1010cafe));
    uart0_print("\r\n");
    uart0_print(_utob(&buf[0], 0xa1b2c3e4));
    uart0_print("\r\n");
#endif

    while (1) {
        // sleep
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        //led_switch;
        check_events();
    }
}
