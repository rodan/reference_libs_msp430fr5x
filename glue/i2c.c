
#include "driverlib.h"
#include "i2c.h"

void i2c_transfer_start(const uint16_t base_addr, const i2c_package_t * pkg,
                        void (*callback) (i2c_status_t result))
{
    uint8_t i;
            

    if (pkg->options & I2C_READ) {
        // some devices need to write a register address/command before a read
        if (pkg->addr_len) {
            EUSCI_B_I2C_setSlaveAddress(base_addr, pkg->slave_addr);
            EUSCI_B_I2C_setMode(base_addr, EUSCI_B_I2C_TRANSMIT_MODE);
            EUSCI_B_I2C_enable(base_addr);
            //__delay_cycles(100);
            // send START, slave address
            EUSCI_B_I2C_masterSendStart(base_addr);

            for (i = 0; i < pkg->addr_len; i++) {
                if (i == 0) {
                    EUSCI_B_I2C_masterSendMultiByteStart(base_addr, pkg->addr[0]);
                } else {
                    EUSCI_B_I2C_masterSendMultiByteNext(base_addr, pkg->addr[i]);
                }
            }
            // send STOP after address/command
            EUSCI_B_I2C_masterSendMultiByteStop(base_addr);
        }

        // SLAVE ADDR + R, read into pkg->data pkg->data_len times
        EUSCI_B_I2C_setSlaveAddress(base_addr, pkg->slave_addr);
        EUSCI_B_I2C_setMode(base_addr, EUSCI_B_I2C_RECEIVE_MODE);
        EUSCI_B_I2C_enable(base_addr);
        //__delay_cycles(100);
        // send slave address
        EUSCI_B_I2C_masterReceiveStart(base_addr);
        for (i = 0; i < pkg->data_len - 1; i++) {
            pkg->data[i] = EUSCI_B_I2C_masterReceiveSingle(base_addr);
            //pkg->data[i] = EUSCI_B_I2C_masterReceiveMultiByteNext(base_addr);
        }
        pkg->data[pkg->data_len - 1] = EUSCI_B_I2C_masterReceiveMultiByteFinish(base_addr);

    } else if (pkg->options & I2C_WRITE) {
        EUSCI_B_I2C_setSlaveAddress(base_addr, pkg->slave_addr);
        EUSCI_B_I2C_setMode(base_addr, EUSCI_B_I2C_TRANSMIT_MODE);
        EUSCI_B_I2C_enable(base_addr);
        //__delay_cycles(100);
        // send START, slave address
        EUSCI_B_I2C_masterSendStart(base_addr);

        for (i = 0; i < pkg->addr_len; i++) {
            if (i == 0) {
                EUSCI_B_I2C_masterSendMultiByteStart(base_addr, pkg->addr[0]);
            } else {
                EUSCI_B_I2C_masterSendMultiByteNext(base_addr, pkg->addr[i]);
            }
        }

        for (i = 0; i < pkg->data_len; i++) {
            if (i == 0) {
                if (pkg->addr_len) {
                    // we already did a SendMultiByteStart
                    EUSCI_B_I2C_masterSendMultiByteNext(base_addr, pkg->data[0]);
                } else {
                    EUSCI_B_I2C_masterSendMultiByteStart(base_addr, pkg->data[0]);
                }
            } else {
                EUSCI_B_I2C_masterSendMultiByteNext(base_addr, pkg->data[i]);
            }
        }
        // send STOP
        EUSCI_B_I2C_masterSendMultiByteStop(base_addr);
    }
        
    //__delay_cycles(400);
    //EUSCI_B_I2C_disable(base_addr);
}
