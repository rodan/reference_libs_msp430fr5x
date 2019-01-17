
#include <msp430.h>
#include "main.h"

#include "cs.h"

void main_init(void)
{
    // port init
    P1DIR = BIT0;

    // XT1
    PJSEL0 |= BIT4 | BIT5;                  // For XT1
    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers
    CSCTL4 &= ~LFXTOFF;
    do
    {
        CSCTL5 &= ~LFXTOFFG;                // Clear XT1 fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);              // Test oscillator fault flag
    CSCTL0_H = 0;                           // Lock CS registers
}

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    main_init();

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    led_off;

    while (1) {
        // sleep
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
    }

}

