
//  code for GT9XX touch IC
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/
//  license:         BSD

/*
 *  Driver for Goodix Touchscreens
 *
 *  Copyright (c) 2014 Red Hat Inc.
 *  Copyright (c) 2015 K. Merker <merker@debian.org>
 *
 *  This code is based on gt9xx.c authored by andrew@goodix.com:
 *
 *  2010 - 2012 Goodix Technology.
 */

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

#include "helper.h"
#include "timer_a1.h"
#include "glue.h"
#include "gt9xx.h"


#ifdef GT9XX_CONF_VER_A
// Hantronix LCD (480x800)
uint8_t gt9xx_conf[GT9XX_CONFIG_911_SZ] =
    { 0x41, 0x20, 0x03, 0xe0, 0x01, 0x05, 0x3d, 0x0, 0x01, 0x08,
    0x1e, 0x05, 0x50, 0x3c, 0x03, 0x05, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x1a, 0x1c, 0x1e, 0x14, 0x8c, 0x2e, 0x0e,
    0x28, 0x2a, 0x5e, 0x05, 0x0, 0x0, 0x01, 0x9b, 0x03, 0x11,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x03, 0x64, 0x32, 0x0, 0x0,
    0x0, 0x19, 0x41, 0x94, 0xc5, 0x02, 0x07, 0x0, 0x0, 0x04,
    0xb9, 0x1b, 0x0, 0x99, 0x21, 0x0, 0x7f, 0x28, 0x0, 0x69,
    0x31, 0x0, 0x59, 0x3b, 0x0, 0x59, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x1c, 0x1a, 0x18, 0x16, 0x14, 0x12, 0x10, 0x0e,
    0x0c, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x2a, 0x29, 0x28, 0x26, 0x24, 0x22, 0x21, 0x20,
    0x1f, 0x1e, 0x1d, 0x1c, 0x18, 0x16, 0x14, 0x13, 0x12, 0x10,
    0x0f, 0x0c, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0xef, 0x1,
};
#elif defined(GT9XX_CONF_VER_B)
// Changes over handtronix version
// version   = 0x42
// touch_num = 0x03
// checksum  = 0xf0
uint8_t gt9xx_conf[GT9XX_CONFIG_911_SZ] = 
    { 0x42, 0x20, 0x03, 0xe0, 0x01, 0x03, 0x3d, 0x0, 0x01, 0x08, 
    0x1e, 0x05, 0x50, 0x3c, 0x03, 0x05, 0x0, 0x0, 0x0, 0x0,     // 10
    0x0, 0x0, 0x0, 0x1a, 0x1c, 0x1e, 0x14, 0x8c, 0x2e, 0x0e,    // 20
    0x28, 0x2a, 0x5e, 0x05, 0x0, 0x0, 0x01, 0x9b, 0x03, 0x11,   // 30
    0x0, 0x0, 0x0, 0x0, 0x0, 0x03, 0x64, 0x32, 0x0, 0x0,        // 40
    0x0, 0x19, 0x41, 0x94, 0xc5, 0x02, 0x07, 0x0, 0x0, 0x04,    // 50
    0xb9, 0x1b, 0x0, 0x99, 0x21, 0x0, 0x7f, 0x28, 0x0, 0x69,    // 60
    0x31, 0x0, 0x59, 0x3b, 0x0, 0x59, 0x0, 0x0, 0x0, 0x0,       // 70
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 80
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 90
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 100
    0x0, 0x0, 0x1c, 0x1a, 0x18, 0x16, 0x14, 0x12, 0x10, 0x0e,   // 110
    0x0c, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x0, 0x0, 0x0, 0x0,     // 120
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 130
    0x0, 0x0, 0x2a, 0x29, 0x28, 0x26, 0x24, 0x22, 0x21, 0x20,   // 140 - 149
    0x1f, 0x1e, 0x1d, 0x1c, 0x18, 0x16, 0x14, 0x13, 0x12, 0x10, // 150 - 159
    0x0f, 0x0c, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x0, 0x0, 0x0,    // 160 - 169
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 170 - 179
    0x0, 0x0, 0x0, 0x0, 0xf0, 0x01                              // 180 - 185
};
#elif defined(GT9XX_CONF_VER_C)
// CCT LCD (480x800 with 3 capacitive keys)
uint8_t gt9xx_conf[GT9XX_CONFIG_911_SZ] = 
    { 0x44, 0x20, 0x03, 0xe0, 0x01, 0x05, 0x3d, 0x01, 0x01, 0x18, 
    0x1e, 0x0f, 0x55, 0x3c, 0x03, 0x05, 0x0, 0x0, 0x0, 0x0,     // 10 
    0x0, 0x0, 0x06, 0x18, 0x1a, 0x1e, 0x14, 0x8c, 0x2c, 0x0e,   // 20
    0x32, 0x34, 0xd3, 0x07, 0x0, 0x0, 0x0, 0x1a, 0x02, 0x2c,    // 30 
    0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x64, 0x32, 0x0, 0x0,        // 40
    0x0, 0x1e, 0x50, 0x94, 0xc5, 0x02, 0x0, 0x14, 0x0, 0x04,    // 50
    0xb4, 0x21, 0x0, 0x99, 0x28, 0x0, 0x82, 0x31, 0x0, 0x70,    // 60
    0x3b, 0x0, 0x62, 0x48, 0x0, 0x62, 0x38, 0x50, 0x68, 0x0,    // 70
    0xf0, 0x4a, 0x3a, 0xff, 0xff, 0x27, 0x0, 0x0, 0x0, 0x0,     // 80 - 89
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 90 - 99
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 100 - 109
    0x0, 0x0, 0x1c, 0x1a, 0x18, 0x16, 0x14, 0x12, 0x10, 0x0e,   // 110 - 119
    0x0c, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x0, 0x0, 0x0, 0x0,     // 120 - 129
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 130 - 139
    0x0, 0x0, 0x28, 0x26, 0x24, 0x22, 0x21, 0x20, 0x1f, 0x1e,   // 140 - 149
    0x1d, 0x1c, 0x18, 0x16, 0x14, 0x13, 0x12, 0x10, 0x0f, 0x0c, // 150 - 159
    0x0a, 0x08, 0x06, 0x04, 0x02, 0x0, 0x29, 0x2a, 0x0, 0x0,    // 160 - 169
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 170 - 179
    0x0, 0x0, 0x0, 0x0, 0x8d, 0x01                              // 180 - 185
};
#elif defined(GT9XX_CONF_VER_D)
// changes over the CCT screen
// 0x8047 0x44 (version)
// 0x804c 0x03 (touch num)
// 0x804e 0x00 (disable touch keys)
// 0x8093-0x809c (disable touch keys-related registers)
// 0x809c 0xf0 (checksum)

uint8_t gt9xx_conf[GT9XX_CONFIG_911_SZ] = 
    { 0x44, 0x20, 0x03, 0xe0, 0x01, 0x03, 0x3d, 0x00, 0x01, 0x18, 
    0x1e, 0x0f, 0x55, 0x3c, 0x03, 0x05, 0x0, 0x0, 0x0, 0x0,     // 10 - 19 
    0x0, 0x0, 0x06, 0x18, 0x1a, 0x1e, 0x14, 0x8c, 0x2c, 0x0e,   // 20 - 29
    0x32, 0x34, 0xd3, 0x07, 0x0, 0x0, 0x0, 0x1a, 0x02, 0x2c,    // 30 - 39
    0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x64, 0x32, 0x0, 0x0,        // 40 - 49
    0x0, 0x1e, 0x50, 0x94, 0xc5, 0x02, 0x0, 0x14, 0x0, 0x04,    // 50 - 59
    0xb4, 0x21, 0x0, 0x99, 0x28, 0x0, 0x82, 0x31, 0x0, 0x70,    // 60 - 69
    0x3b, 0x0, 0x62, 0x48, 0x0, 0x62, 0x0, 0x0, 0x0, 0x0,       // 70 - 79
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 80 - 89
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 90 - 99
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 100 - 109
    0x0, 0x0, 0x1c, 0x1a, 0x18, 0x16, 0x14, 0x12, 0x10, 0x0e,   // 110 - 119
    0x0c, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x0, 0x0, 0x0, 0x0,     // 120 - 129
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 130 - 139
    0x0, 0x0, 0x28, 0x26, 0x24, 0x22, 0x21, 0x20, 0x1f, 0x1e,   // 140 - 149
    0x1d, 0x1c, 0x18, 0x16, 0x14, 0x13, 0x12, 0x10, 0x0f, 0x0c, // 150 - 159
    0x0a, 0x08, 0x06, 0x04, 0x02, 0x0, 0x29, 0x2a, 0x0, 0x0,    // 160 - 169
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,           // 170 - 179
    0x0, 0x0, 0x0, 0x0, 0x18, 0x01                              // 180 - 185
};
#endif

uint8_t gt9xx_coord_buff[GT9XX_COORD_MAX_COUNT * GT9XX_POINT_STRUCT_SZ];
volatile uint8_t gt9xx_last_event;
uint8_t gt9xx_debug_status[GT9XX_DEBUG_BUFFER_SZ];

// pointer to the struct being handled by the configured IRQ request
struct goodix_ts_data *ts_handled;

void (*GT9XX_HLHandler)(struct GT9XX_coord_t*);

static void gt9xx_event_handler(uint32_t msg)
{
    sig1_switch;
    GT9XX_disable_irq();
    GT9XX_read_state(ts_handled, gt9xx_coord_buff);
    GT9XX_enable_irq();
}

void GT9XX_rst_event(void)
{
    gt9xx_last_event = GT9XX_EV_NULL;
}

uint8_t GT9XX_get_event(void)
{
    return gt9xx_last_event;
}

uint8_t GT9XX_read(struct goodix_ts_data * t, const uint16_t reg,
                   uint8_t * buf, const size_t buf_len)
{
    uint8_t rv = 0;
    static i2c_package_t pkg;
    static uint8_t addr[2];

    addr[0] = (reg & 0xff00) >> 8;
    addr[1] = reg & 0xff;

    pkg.slave_addr = t->slave_addr;
    pkg.addr = addr;
    pkg.addr_len = 2;
    pkg.data = buf;
    pkg.data_len = buf_len;
    pkg.options = I2C_READ | I2C_LAST_NAK;

    i2c_transfer_start(t->usci_base_addr, &pkg, NULL);

    return rv;
}

uint8_t GT9XX_write(struct goodix_ts_data * t, const uint16_t reg,
                    uint8_t * buf, const size_t buf_len)
{
    uint8_t rv = 0;
    static i2c_package_t pkg;
    static uint8_t addr[2];

    addr[0] = (reg & 0xff00) >> 8;
    addr[1] = reg & 0xff;

    pkg.slave_addr = t->slave_addr;
    pkg.addr = addr;
    pkg.addr_len = 2;
    pkg.data = buf;
    pkg.data_len = buf_len;
    pkg.options = I2C_WRITE;

    i2c_transfer_start(t->usci_base_addr, &pkg, NULL);

    return rv;
}

int16_t GT9xx_check_conf_8(const struct firmware * cfg)
{
    int i, raw_cfg_len = cfg->size - 2;
    uint8_t checksum = 0;

    for (i = 0; i < raw_cfg_len; i++) {
        checksum += cfg->data[i];
    }
    checksum = (~checksum) + 1;

#ifdef OVERWRITE_CHECKSUM
    cfg->data[raw_cfg_len] = checksum;
#else
    if (checksum != cfg->data[raw_cfg_len]) {
        // wrong checksum
        return GT9XX_err_check_cfg_checksum;
    }
#endif

    if (cfg->data[raw_cfg_len + 1] != 1) {
        // config must have Config_Fresh register set
        return GT9XX_err_check_cfg_fresh;
    }

    return 0;
}

uint8_t GT9XX_calc_checksum(uint8_t * buf, uint16_t len)
{
    uint8_t ret = 0;
    uint8_t i;

    for (i = len - 1; i > 0; i--) {
        ret += buf[i - 1];
    }
    ret = (~ret) + 1;
    return ret;
}

uint8_t GT9XX_init(struct goodix_ts_data * t)
{
    uint8_t rv = 0;

    // stay a while and listen
    timer_a1_delay_ccr2(_200ms);
    timer_a1_delay_ccr2(_100ms);

    // reset

    // set both control pins as output, low level
    set_irq_output;
    set_irq_low;
    set_rst_output;
    set_rst_low;

    // select the slave address
    // T2 > 10ms
    timer_a1_delay_ccr2(11 * _1ms);

    // HIGH: 0x14, LOW (default): 0x5d
    if (t->slave_addr == 0x14) {
        set_irq_high;
    }
    // T3 > 100us
    timer_a1_delay_ccr2(2 * _100us);
    set_rst_high;
    //set_rst_input;

    // T4 > 5ms
    timer_a1_delay_ccr2(6 * _1ms);
    // set_irq_low; // this will actually set a pulldown resistor
    // slave address selection has ended

    // T5 > 50ms
    timer_a1_delay_ccr2(51 * _1ms);
    set_irq_input;

    timer_a1_delay_ccr2(_200ms);

    // send configuration

    ts_handled = t;             // kinda cludgy
    eh_register(&gt9xx_event_handler, SYS_MSG_GT9XX_IRQ);

    rv = GT9XX_read_chip_id(t);
    if (rv) {
        return rv;
    }

    rv = GT9XX_check_chip_id(t->chip_id);
    if (rv) {
        return GT9XX_err_check_chip_id;
    }

    t->conf.data = NULL;

    rv = GT9XX_read_conf_version(t);
    if (rv) {
        return rv;
    }

/*
    rv = GT9XX_check_conf_version(t);
    if (rv == GT9XX_NEED_CONF_UPDATE) {
        rv = GT9XX_write_config(t, (uint8_t *) &gt9xx_conf, GT9XX_CONFIG_911_SZ);
        if (rv) {
            return rv;
        }
    } else if (rv != EXIT_SUCCESS) {
        return rv;
    }
*/
    return 0;
}

uint8_t GT9XX_clear_irq(struct goodix_ts_data * t)
{
    uint8_t rv = 0;
    static uint8_t data[1] = { 0 };

    rv = GT9XX_write(t, GT9XX_rCOORD_ADDR, (uint8_t *) data, 1);
    return rv;
}

// returns EXIT_FAILURE if chip is not supported
uint8_t GT9XX_check_chip_id(const uint16_t id)
{
    switch (id) {
    case 911:
    case 9110:
    case 927:
    case 9271:
    case 928:
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

// reads 2 bytes from the configuration area - the conf version and the checksum
// and places the data into t->conf.*
uint8_t GT9XX_read_chip_id(struct goodix_ts_data * t)
{
    uint8_t rv = EXIT_FAILURE;
    static char buf[11];

    rv = GT9XX_read(t, GT9XX_rDATA, (uint8_t *) buf, sizeof(buf));
    if (rv) {
        return GT9XX_err_i2c_read;
    }

    t->chip_id = _strtou16(buf);

    return EXIT_SUCCESS;
}

// checks the current config parameters from t->conf against the provided data
uint8_t GT9XX_check_conf_version(struct goodix_ts_data * t)
{
    if ((t->conf.version < gt9xx_conf[rOFF_CONF_VER]) || ((t->conf.version == gt9xx_conf[rOFF_CONF_VER]) && (t->conf.checksum != gt9xx_conf[rOFF_CHECKSUM]))) {
        return GT9XX_NEED_CONF_UPDATE;
    }

    if ((t->conf.version == gt9xx_conf[rOFF_CONF_VER]) && (t->conf.checksum == gt9xx_conf[rOFF_CHECKSUM])) {
        // no need to update
        return EXIT_SUCCESS;
    }

    return GT9XX_err_check_cfg_ver;
}

// reads 2 bytes from the configuration area - the conf version and the checksum
// and places the data into t->conf.*
uint8_t GT9XX_read_conf_version(struct goodix_ts_data * t)
{
    uint8_t rv = EXIT_FAILURE;
    static char buf[1];

    rv = GT9XX_read(t, GT9XX_rCFG + rOFF_CONF_VER, (uint8_t *) buf, 1);
    if (rv) {
        return GT9XX_err_i2c_read;
    }
    t->conf.version = buf[0];

    rv = GT9XX_read(t, GT9XX_rCFG + rOFF_CHECKSUM, (uint8_t *) buf, 1);
    if (rv) {
        return GT9XX_err_i2c_read;
    }
    t->conf.checksum = buf[0];

    return EXIT_SUCCESS;
}

// reads all GT9XX_CONFIG_911_SZ bytes from GT9XX_rCFG and places the data into
// t->conf.*
uint8_t GT9XX_read_config(struct goodix_ts_data * t)
{
    uint8_t *config_data;
    int16_t rv = EXIT_SUCCESS;

    if ((t->conf.data == NULL) || (t->conf.size != GT9XX_CONFIG_911_SZ)) {
        config_data = (uint8_t *) malloc(sizeof(uint8_t) * GT9XX_CONFIG_911_SZ);
        if (config_data == NULL) {
            return GT9XX_err_malloc;
        }
        t->conf.size = GT9XX_CONFIG_911_SZ;
        t->conf.data = config_data;
    }

    rv = GT9XX_read(t, GT9XX_rCFG, t->conf.data, GT9XX_CONFIG_911_SZ);
    if (rv) {
        return GT9XX_err_i2c_read;
    }

    t->conf.version  = t->conf.data[rOFF_CONF_VER];
    t->conf.checksum = (uint8_t) t->conf.data[rOFF_CHECKSUM]; // 911 has a 8bit checksum, should ifdef?

    return EXIT_SUCCESS;
}

uint8_t GT9XX_write_config(struct goodix_ts_data * t, uint8_t *data, size_t data_len)
{
    int16_t rv = EXIT_SUCCESS;

    struct firmware new_conf;

    new_conf.size = data_len;
    new_conf.data = data;

    rv = GT9xx_check_conf_8(&new_conf);
    if (rv) {
        return rv;
    }

    rv = GT9XX_write(t, GT9XX_rCFG, data, data_len);
    if (rv) {
        return GT9XX_err_i2c_write;
    }
    // allow the TS chip to reconfigure itself
    timer_a1_delay_ccr2(_200ms);

    return EXIT_SUCCESS;
}

void GT9XX_free_config(struct goodix_ts_data *t)
{
    if (t->conf.data != NULL) {
        t->conf.size = 0;
        free(t->conf.data);
    }
}

// returns either a positive number with the number of contacts or a negative number if an error occured
int16_t GT9XX_read_state(struct goodix_ts_data *t, uint8_t * data)
{
    uint8_t rv = 0;
    uint8_t ret;
    uint8_t coord_count = 0;
    static uint8_t reply[1];
    uint8_t should_callback = 0;
    //char itoa_buf[18];

    rv = GT9XX_read(t, GT9XX_rCOORD_ADDR, reply, 1);
    if (rv) {
        return -rv;
    }

    if (!(reply[0] & 0x80)) {
        // no coordinates available
        return -1;
    }
    ret = reply[0] & 0x1f; // bit0-3 is the number of detected touches, bit 4 represents 'key present'

    //uart0_print(_utoh(itoa_buf, reply[0]));
    //uart0_print("\r\n");

#ifdef DEBUG_STATUS
    rv = GT9XX_read(t, GT9XX_rCOORD_ADDR + 1, gt9xx_debug_status, GT9XX_DEBUG_BUFFER_SZ);
    should_callback = 1;

#else

    coord_count = reply[0] & 0xf;
    if (ret > 0 && ret <= GT9XX_COORD_MAX_COUNT) {
        rv = GT9XX_read(t, GT9XX_rCOORD_ADDR + 1, (uint8_t *) &t->coord.point,
                        GT9XX_POINT_STRUCT_SZ * (ret));
        if (rv) {
            return rv;
        }
        t->coord.count = ret;
        should_callback = 1;
    }

    if (coord_count > 0 && coord_count <= GT9XX_COORD_MAX_COUNT) {
        rv = GT9XX_read(t, GT9XX_rCOORD_ADDR + 1, (uint8_t *) &t->coord.point,
                        GT9XX_POINT_STRUCT_SZ * (coord_count));
        if (rv) {
            return rv;
        }
        t->coord.count = coord_count;
        should_callback = 1;
    }

    if (ret & 0x10) {
        rv = GT9XX_read(t, GT9XX_rKEY_ADDR, (uint8_t *) &t->coord.key,1);
        should_callback = 1;
    } else {
        t->coord.key = 0;
    }

    if (ret == 0x10) {
        t->coord.count = 0;
    }
#endif
    // zero out GT9XX_rCOORD_ADDR
    GT9XX_clear_irq(t);

    if (should_callback) {
        if (GT9XX_HLHandler != NULL) {
            GT9XX_HLHandler(&t->coord);
        }
    }

    return ret;
}

void GT9XX_disable_irq(void)
{
    P6IE &= ~GT9XX_IRQ;         // disable interrupt
    P6IFG &= ~GT9XX_IRQ;        // clear interrupt
}

void GT9XX_enable_irq(void)
{
    P6IFG &= ~GT9XX_IRQ;        // clear interrupt
    P6IE |= GT9XX_IRQ;          // enable interrupt
}

uint16_t _strtou16(char *buf)
{
    uint16_t val = 0;
    uint8_t c;
    uint8_t i;

    if ((buf[0] < 0x30) || (buf[0] > 0x46) || (buf[0] == 0x40)) {
        return 0;
    }

    for (i = 0; i < 4; i++) {
        c = buf[i];
        if (c == 0) {
            return val;
        }
        if (c < 0x40) {
            c -= 48;
        } else {
            c -= 55;
        }
        val = val * 10 + c;
    }
    return val;
}

void GT9XX_set_HLHandler(void (*handler)(struct GT9XX_coord_t*)) {
    GT9XX_HLHandler = handler;
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
