
#include <msp430.h>
#include "proj.h"
#include "config.h"
#include "driverlib.h"

void main_init(void);

int main(void)
{
    main_init();

    while (1) {
        // sleep
        //_BIS_SR(LPM3_bits + GIE);
        __no_operation();
        __delay_cycles(8000000);
        led_switch;
    }
}

void main_init(void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

#ifdef USE_XT1
    PJSEL0 |= BIT4 | BIT5;
    CS_setExternalClockSource(32768,0);
#endif

    // Set DCO Frequency to 8MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);

    // configure MCLK, SMCLK to be source by DCOCLK
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

#ifdef USE_XT1
    CS_turnOnLFXT(CS_LFXT_DRIVE_0);
#endif

    // * port init
    //    LED
    P1DIR = BIT0;

    //    output SMCLK on P3.4
    P3OUT &= ~BIT4;
    P3DIR |= BIT4;
    P3SEL1 |= BIT4;

    PMM_unlockLPM5();

    led_off;
}


