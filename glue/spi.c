
#include <msp430.h>
#include <inttypes.h>
#include "driverlib.h"

void spi_read_frame(const uint16_t baseAddress, uint8_t * pBuffer, uint16_t size)
{
    uint16_t gie = _get_SR_register() & GIE;   //Store current GIE state

    __disable_interrupt();      //Make this operation atomic

    //UCB1IFG &= ~UCRXIFG;        
    // ensure RXIFG is clear
    EUSCI_B_SPI_clearInterrupt(baseAddress, UCRXIFG);

    //Clock the actual data transfer and receive the bytes
    while (size--) {
        //while (!(UCB1IFG & UCTXIFG)) ;  
        // wait while not ready for TX
        while (!(EUSCI_B_SPI_getInterruptStatus(baseAddress, UCTXIFG))) {};
        //UCB1TXBUF = 0xff;
        // write dummy byte
        EUSCI_B_SPI_transmitData(baseAddress, 0xff);
        //while (!(UCB1IFG & UCRXIFG)) ;  //Wait for RX buffer (full)
        while (!(EUSCI_B_SPI_getInterruptStatus(baseAddress, UCRXIFG))) {};  //Wait for RX buffer (full)
        *pBuffer++ = EUSCI_B_SPI_receiveData(baseAddress);
    }

    _bis_SR_register(gie);          //Restore original GIE state    
}

void spi_send_frame(const uint16_t baseAddress, uint8_t * pBuffer, uint16_t size)
{
    uint16_t gie = _get_SR_register() & GIE;   //Store current GIE state

    __disable_interrupt();      //Make this operation atomic

    //Clock the actual data transfer and send the bytes. Note that we
    //intentionally not read out the receive buffer during frame transmission
    //in order to optimize transfer speed, however we need to take care of the
    //resulting overrun condition.
    while (size--) {
        // wait while not ready for TX
        while (!(EUSCI_B_SPI_getInterruptStatus(baseAddress, UCTXIFG))) {};

        //while (!(UCB1IFG & UCTXIFG)) ;  //Wait while not ready for TX
        //UCB1TXBUF = *pBuffer++; //Write byte
        EUSCI_B_SPI_transmitData(baseAddress, *pBuffer++);
    }
    //while (UCB1STAT & UCBUSY) ; //Wait for all TX/RX to finish
    while (EUSCI_B_SPI_isBusy(baseAddress)) {};

    //UCB1RXBUF;                  //Dummy read to empty RX buffer
    //and clear any overrun conditions
    EUSCI_B_SPI_receiveData(baseAddress);

    _bis_SR_register(gie);          //Restore original GIE state
}

