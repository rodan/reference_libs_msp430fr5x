
//  code for GT9XX touch IC
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/
//  license:         BSD
//
//  Honeywell High Accuracy Ceramic (HSC) and Standard Accuracy Ceramic
//  (SSC) Series are piezoresistive silicon pressure sensors.

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

#include "uart0.h"
#include "helper.h"
#include "timer_a1.h"
#include "sys_messagebus.h"
#include "gt9xx.h"

//#define READ_CONFIG_DURING_INIT

// factory config
    uint8_t gt9xx_conf[GT9XX_CONFIG_911_LENGTH] =
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
          0x0, 0x0, 0x0, 0x0, 0xef, 0x0,
    };

uint8_t gt9xx_coord_buff[GT9XX_COORD_MAX_COUNT * GT9XX_POINT_STRUCT_SZ];

volatile uint8_t gt9xx_last_event;

// pointer to the struct being handled by the configured IRQ request
struct goodix_ts_data *ts_handled;

static void gt9xx_event_handler(uint16_t msg)
{
    sig1_switch;
    GT9XX_disable_irq();
    GT9XX_read_state(ts_handled, gt9xx_coord_buff);
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

uint8_t GT9XX_read(struct goodix_ts_data *t, const uint16_t reg,
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

uint8_t GT9XX_write(struct goodix_ts_data *t, const uint16_t reg,
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

int16_t GT9xx_check_cfg_8(struct goodix_ts_data *t,
			const struct firmware *cfg)
{
	int i, raw_cfg_len = cfg->size - 2;
	uint8_t check_sum = 0;

	for (i = 0; i < raw_cfg_len; i++)
		check_sum += cfg->data[i];
	check_sum = (~check_sum) + 1;

	if (check_sum != cfg->data[raw_cfg_len]) {
        // wrong checksum
		return GT9XX_err_check_cfg_checksum;
	}

	if (cfg->data[raw_cfg_len + 1] != 1) {
	    // config must have Config_Fresh register set
		return GT9XX_err_check_cfg_fresh;
	}

	return 0;
}

uint8_t GT9XX_calc_checksum(uint8_t* buf, uint16_t len) {
  uint8_t ret = 0;
  uint8_t i;

  for (i = len - 1; i > 0; i--) {
    ret += buf[i-1];
  }
  ret = (~ret) + 1;
  return ret;
}

uint8_t GT9XX_init(struct goodix_ts_data *t)
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

    timer_a1_delay_ccr2(51*_1ms);

    // send configuration

    ts_handled = t; // kinda cludgy
    sys_messagebus_register(&gt9xx_event_handler, SYS_MSG_GT9XX_IRQ);

    rv = GT9XX_read_version(t);
    if (rv) {
        return rv;
    }

    rv = GT9XX_check_chipid(t->id);
    if (rv) {
        return GT9XX_err_check_chip_id;
    }

#ifdef READ_CONFIG_DURING_INIT
    // optionally read the config registers (this will malloc() 186 bytes)
    rv = GT9XX_read_config(t);
    if (rv) {
        return rv;
    }

    // fake a fresh config so it's test passes the _check_config()
    t->conf.data[rOFF_FRESH_CONFIG] = 1;

    rv = GT9xx_check_cfg_8(t, &t->conf);
    if (rv) {
        return rv;
    }
#endif

    return rv;
}

uint8_t GT9XX_clear_irq(struct goodix_ts_data *t)
{
    uint8_t rv = 0;
    uint8_t data[1] = { 0 };

    rv = GT9XX_write(t, GT9XX_rCOORD_ADDR, (uint8_t *) data, 1);
    return rv;
}

// returns EXIT_FAILURE if chip is not supported
uint8_t GT9XX_check_chipid(const uint16_t id)
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

uint8_t GT9XX_read_version(struct goodix_ts_data *t)
{
    uint8_t rv = EXIT_FAILURE;
    char buf[11];
    //struct GTInfo *gt_info;

    rv = GT9XX_read(t, GT9XX_rDATA, (uint8_t *) buf, sizeof(buf));
    if (rv) {
        return GT9XX_err_i2c_read;
    }

    t->id = _strtou16(buf);

    return rv;
}

uint8_t GT9XX_read_config(struct goodix_ts_data *t)
{
    uint8_t *config_data;
    int16_t rv = EXIT_SUCCESS;
    
    config_data = (uint8_t *) malloc(sizeof(uint8_t)*GT9XX_CONFIG_911_LENGTH);
    if (config_data == NULL) {
        return GT9XX_err_malloc;
    }

    t->conf.size = GT9XX_CONFIG_911_LENGTH;
    t->conf.data = config_data;

    rv = GT9XX_read(t, GT9XX_rCFG, t->conf.data, GT9XX_CONFIG_911_LENGTH);
    if (rv) {
        return GT9XX_err_i2c_read;
    }

    return rv;
}

uint8_t GT9XX_write_config(struct goodix_ts_data *t)
{
    int16_t rv = EXIT_SUCCESS;

    rv = GT9xx_check_cfg_8(t, &t->conf);
    if (rv) {
        return rv;
    }

    rv = GT9XX_write(t, GT9XX_rCFG, t->conf.data, GT9XX_CONFIG_911_LENGTH);
    if (rv) {
        return GT9XX_err_i2c_write;
    }

    return rv;
}

void GT9XX_free_config(struct goodix_ts_data *t)
{
    if (t->conf.data != NULL) {
        t->conf.size = 0;
        free(t->conf.data);
    }
}

int16_t GT9XX_read_state(struct goodix_ts_data *t, uint8_t * data)
{
    uint8_t rv = 0;
    uint8_t coord_cnt;
    char itoa_buf[18];
    uint8_t reply[1];
    uint8_t i;
    struct GTPoint *coord;

    coord = (struct GTPoint *) gt9xx_coord_buff;

    rv = GT9XX_read(t, GT9XX_rCOORD_ADDR, reply, 1);

    if (rv) {
        // i2c error
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

        rv = GT9XX_read(t, GT9XX_rCOORD_ADDR + 1, gt9xx_coord_buff, GT9XX_POINT_STRUCT_SZ * (coord_cnt));

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

    GT9XX_clear_irq(t);

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

uint16_t _strtou16(char *buf) {
    uint16_t val = 0;
    uint8_t c;
    uint8_t i;

    if ( (buf[0] < 0x30) || (buf[0] > 0x46) || ( buf[0] == 0x40 ) ) {
        return 0;
    }

    for (i=0; i<4; i++) {
        c = buf[i];
        if (c == 0) {
            return val;
        }
        if (c < 0x40) {
            c -= 48;
        } else {
            c -= 55;
        }
        val = val*10 + c;
    }
    return val;
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
