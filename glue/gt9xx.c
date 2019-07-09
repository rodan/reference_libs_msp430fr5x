
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
#include "gt9xx.h"

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
    int touch_num;
    char itoa_buf[18];
    uint8_t reply[1];

    rv = GT9XX_read(usci_base_addr, slave_addr, GOODIX_READ_COORD_ADDR, reply, 1);
    uart0_print("reply: ");
    uart0_print(_utoh(itoa_buf, reply[0]));
    uart0_print("\n");

    if (rv) {
        uart0_print("i2c err\n");
        //dev_err(&ts->client->dev, "I2C transfer error: %d\n", error);
        return -rv;
    }

    if (!(reply[0] & 0x80)) {
        return -100;
    }

    touch_num = reply[0] & 0x0f;
    uart0_print("touch_num: ");
    uart0_print(_utoh(itoa_buf, touch_num));
    uart0_print("\n");

    //if (touch_num > ts->max_touch_num)
    //  return -EPROTO;

    //log_printf("touch num: %d\n", touch_num);

/*
    if (touch_num > 0) {
    rv = read(GOODIX_READ_COORD_ADDR + 1, data, GOODIX_CONTACT_SIZE * (touch_num));

    if (error)
      return -error;
  }
*/

    return touch_num;
}

#endif                          // __I2C_CONFIG_H__
