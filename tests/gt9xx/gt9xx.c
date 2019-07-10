
//  code for GT9XX touch IC
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/
//  license:         BSD
//
//  Honeywell High Accuracy Ceramic (HSC) and Standard Accuracy Ceramic
//  (SSC) Series are piezoresistive silicon pressure sensors.

#include "config.h"
#ifdef __I2C_CONFIG_H__

#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef HARDWARE_I2C
#include "i2c.h"
#else
#include "serial_bitbang.h"
#endif

#include "uart0.h"
#include "helper.h"
#include "timer_a1.h"
#include "sys_messagebus.h"
#include "gt9xx.h"

uint8_t gt9xx_coord_buff[GT9XX_COORD_MAX_COUNT * GT9XX_POINT_STRUCT_SZ];

volatile uint8_t gt9xx_last_event;

static void gt9xx_event_handler(uint16_t msg)
{
    sig1_switch;
    GT9XX_disable_irq();
    GT9XX_read_state(EUSCI_BASE_ADDR, GT9XX_SA, gt9xx_coord_buff);
    GT9XX_enable_irq();
}

void gt9xx_rst_event(void)
{
    gt9xx_last_event = GT9XX_EV_NULL;
}

uint8_t gt9xx_get_event(void)
{
    return gt9xx_last_event;
}

uint8_t GT9XX_read(const uint16_t usci_base_addr, const uint8_t slave_addr, const uint16_t reg,
                   uint8_t * buf, const size_t buf_len)
{
    uint8_t rv = 0;
    static i2c_package_t pkg;
    static uint8_t addr[2];

    addr[0] = (reg & 0xff00) >> 8;
    addr[1] = reg & 0xff;

    pkg.slave_addr = slave_addr;
    pkg.addr = addr;
    pkg.addr_len = 2;
    pkg.data = buf;
    pkg.data_len = buf_len;
    pkg.options = I2C_READ | I2C_LAST_NAK;

#ifdef HARDWARE_I2C
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#else
    rv = i2cm_transfer(&pkg);

    if (rv != I2C_ACK) {
        return rv;
    }
#endif

    return rv;
}

uint8_t GT9XX_write(const uint16_t usci_base_addr, const uint8_t slave_addr, const uint16_t reg,
                    uint8_t * buf, const size_t buf_len)
{
    uint8_t rv = 0;
    static i2c_package_t pkg;
    static uint8_t addr[2];

    addr[0] = (reg & 0xff00) >> 8;
    addr[1] = reg & 0xff;

    pkg.slave_addr = slave_addr;
    pkg.addr = addr;
    pkg.addr_len = 2;
    pkg.data = buf;
    pkg.data_len = buf_len;
    pkg.options = I2C_WRITE;

#ifdef HARDWARE_I2C
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#else
    rv = i2cm_transfer(&pkg);

    if (rv != I2C_ACK) {
        return rv;
    }
#endif

    return rv;
}

// returns  0 if all is fine
//          1 if chip is in command mode
//          2 if old data is being read
//          3 if a diagnostic fault is triggered in the chip
//          I2C err levels if sensor is not properly hooked up

uint8_t GT9XX_init(const uint16_t usci_base_addr, const uint8_t slave_addr)
{
    uint8_t rv = 0;
    struct GTInfo gt_info;

    timer_a1_delay_ccr2(_200ms);
    timer_a1_delay_ccr2(_100ms);

    // reset
    
    // set both control pins as output
    set_irq_output; // interrupt pin
    set_rst_output; //P5DIR |= BIT2;      // rst pin

    // set both signals low
    set_irq_low;
    set_rst_low;

    // select the slave address
    // T2 > 10ms
    timer_a1_delay_ccr2(11 * _1ms);

    // HIGH: 0x28/0x29 (0x14 7bit), LOW: 0xBA/0xBB (0x5D 7bit)
    if (slave_addr == 0x14) {
        set_irq_high;
    }
   
    // T3 > 100us
    timer_a1_delay_ccr2(2*_100us);
    set_rst_high;
    //set_rst_input;

    // T4 > 5ms
    timer_a1_delay_ccr2(6*_1ms);
    // set_irq_low; // this will actually set a pulldown resistor
    // slave address selection has ended

    // T5 > 50ms
    timer_a1_delay_ccr2(51*_1ms);
    set_irq_input;  

    timer_a1_delay_ccr2(_200ms);

    sys_messagebus_register(&gt9xx_event_handler, SYS_MSG_GT9XX_IRQ);

    rv = GT9XX_read(usci_base_addr, slave_addr, GT_REG_DATA, (uint8_t *) & gt_info,
                    sizeof(gt_info));
    return rv;
}

uint8_t GT9XX_clear_irq(const uint16_t usci_base_addr, const uint8_t slave_addr)
{
    uint8_t rv = 0;
    uint8_t data[1] = { 0 };

    rv = GT9XX_write(usci_base_addr, slave_addr, GOODIX_READ_COORD_ADDR, (uint8_t *) data, 1);
    return rv;
}

uint8_t GT9XX_read_info(const uint16_t usci_base_addr, const uint8_t slave_addr)
{
    //uint8_t val[4] = { 0, 0, 0, 0 };
    uint8_t rv = 0;

/*
    i2c_package_t pkg;

    pkg.slave_addr = slave_addr;
    pkg.addr[0] = 0;
    pkg.addr_len = 0;
    pkg.data = val;
    pkg.data_len = 4;
    pkg.options = I2C_READ | I2C_LAST_NAK;

#ifdef HARDWARE_I2C
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#else
    rv = i2cm_transfer(&pkg);

    if (rv != I2C_ACK) {
        return rv;
    }
#endif
*/
/*
    raw->status = (val[0] & 0xc0) >> 6; // first 2 bits from first byte
    raw->bridge_data = ((val[0] & 0x3f) << 8) + val[1];
    raw->temperature_data = ((val[2] << 8) + (val[3] & 0xe0)) >> 5;
*/
    return rv;
}

int16_t GT9XX_read_state(const uint16_t usci_base_addr, const uint8_t slave_addr, uint8_t * data)
{
    uint8_t rv = 0;
    uint8_t coord_cnt;
    char itoa_buf[18];
    uint8_t reply[1];
    uint8_t i;
    struct GTPoint *coord;

    coord = (struct GTPoint *) gt9xx_coord_buff;

    rv = GT9XX_read(usci_base_addr, slave_addr, GOODIX_READ_COORD_ADDR, reply, 1);
    //uart0_print("reply: ");
    //uart0_print(_utoh(itoa_buf, reply[0]));
    //uart0_print("\n");

    if (rv) {
        uart0_print("i2c err\n");
        return -rv;
    }

    if (!(reply[0] & 0x80)) {
        // no coordinates available
        return -100;
    }

    coord_cnt = reply[0] & 0x0f;

    if (coord_cnt > 0 && coord_cnt <= GT9XX_COORD_MAX_COUNT ) {
        uart0_print("c");
        uart0_print(_utoa(itoa_buf, coord_cnt));
        uart0_print("\r\n");

        rv = GT9XX_read(usci_base_addr, slave_addr, GOODIX_READ_COORD_ADDR + 1, gt9xx_coord_buff, GT9XX_POINT_STRUCT_SZ * (coord_cnt));

        for (i=0; i<coord_cnt; i++) {
            uart0_print(" ");
            uart0_print(_utoa(itoa_buf, coord[i].trackId));
            uart0_print(" ");
            uart0_print(_utoa(itoa_buf, coord[i].x));
            uart0_print(" ");
            uart0_print(_utoa(itoa_buf, coord[i].y));
            uart0_print(" ");
            uart0_print(_utoa(itoa_buf, coord[i].area));
            uart0_print("\r\n");
        }

        //if (rv) {
        //    return -rv;
        //}
        uart0_print("\r\n");
    } else {
        uart0_print(".");
    }

    GT9XX_clear_irq(usci_base_addr, slave_addr);

    return coord_cnt;
}

void GT9XX_disable_irq(void)
{
    P6IE &= ~GT9XX_IRQ;  // disable interrupt
    P6IFG &= ~GT9XX_IRQ; // clear interrupt
}

void GT9XX_enable_irq(void)
{
    P6IFG &= ~GT9XX_IRQ; // clear interrupt
    P6IE |= GT9XX_IRQ;  // enable interrupt
}

// Port 6 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT6_VECTOR
__interrupt void Port_6(void)
#elif defined(__GNUC__)
__attribute__ ((interrupt(PORT6_VECTOR)))
void Port6_ISR(void)
#else
#error Compiler not supported!
#endif
{
    if (P6IFG & GT9XX_IRQ) {
        gt9xx_last_event = GT9XX_EV_IRQ;
        P6IFG &= ~GT9XX_IRQ;
        LPM3_EXIT;
    }
}


#endif                          // __I2C_CONFIG_H__
