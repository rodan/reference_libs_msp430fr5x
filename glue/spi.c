
#include <msp430.h>
#include <inttypes.h>
#include "driverlib.h"

void spi_read_frame(const uint16_t baseAddress, uint8_t * pBuffer, uint16_t size)
{
    // store current GIE state
    uint16_t gie = _get_SR_register() & GIE;

    // make this operation atomic
    __disable_interrupt();

    // ensure RXIFG is clear
    EUSCI_B_SPI_clearInterrupt(baseAddress, UCRXIFG);

    // clock the actual data transfer and receive the bytes
    while (size--) {
        // wait while not ready for TX
        while (!(EUSCI_B_SPI_getInterruptStatus(baseAddress, UCTXIFG))) {};
        // write dummy byte
        EUSCI_B_SPI_transmitData(baseAddress, 0xff);
        // wait for RX buffer (full)
        while (!(EUSCI_B_SPI_getInterruptStatus(baseAddress, UCRXIFG))) {};
        *pBuffer++ = EUSCI_B_SPI_receiveData(baseAddress);
    }

    // restore original GIE state
    _bis_SR_register(gie);
}

void spi_send_frame(const uint16_t baseAddress, uint8_t * pBuffer, uint16_t size)
{
    // store current GIE state
    uint16_t gie = _get_SR_register() & GIE;

    // make this operation atomic
    __disable_interrupt();

    // clock the actual data transfer and send the bytes. Note that we
    // intentionally not read out the receive buffer during frame transmission
    // in order to optimize transfer speed, however we need to take care of the
    // resulting overrun condition.
    while (size--) {
        // wait while not ready for TX
        while (!(EUSCI_B_SPI_getInterruptStatus(baseAddress, UCTXIFG))) {};
        EUSCI_B_SPI_transmitData(baseAddress, *pBuffer++);
    }
    while (EUSCI_B_SPI_isBusy(baseAddress)) {};

    // dummy read to empty RX buffer
    // and clear any overrun conditions
    EUSCI_B_SPI_receiveData(baseAddress);

    // restore original GIE state
    _bis_SR_register(gie);
}

