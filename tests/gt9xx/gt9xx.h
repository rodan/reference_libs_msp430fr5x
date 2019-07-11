
/**
* \addtogroup 
* \brief 
* \author Petre Rodan
* 
* \{
**/

#ifndef __GT9XX_H_
#define __GT9XX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "proj.h"

#define assert_rst          P5OUT &= ~BIT2
#define deassert_rst        P5OUT |= BIT2

#define set_irq_output      P6DIR |= GT9XX_IRQ
#define set_irq_input       P6DIR &= ~GT9XX_IRQ

#define set_irq_low         P6OUT &= ~GT9XX_IRQ
#define set_irq_high        P6OUT |= GT9XX_IRQ

#define set_rst_output      P5DIR |= GT9XX_RST
#define set_rst_input       P5DIR &= ~GT9XX_RST

#define set_rst_low         P5OUT &= ~GT9XX_RST
#define set_rst_high        P5OUT |= GT9XX_RST

#define set_rst_input       P5DIR &= ~GT9XX_RST
#define set_rst_output      P5DIR |= GT9XX_RST

// events
#define                  GT9XX_EV_NULL  0
#define                   GT9XX_EV_IRQ  0x1

// register addresses
#define                     GT9XX_rCMD  0x8040
#define                     GT9XX_rCFG  0x8047
#define                    GT9XX_rDATA  0x8140
#define              GT9XX_rCOORD_ADDR  0x814E
#define              GT9XX_rCMD_STATUS  0x81A8

#define          GT9XX_COORD_MAX_COUNT  5

// size of the configuration struct
#define            GT9XX_CONFIG_911_SZ  186
#define          GT9XX_POINT_STRUCT_SZ  8

// relative address for configuration registers
#define                  rOFF_CONF_VER  0
#define                rOFF_RESOLUTION  1
#define              rOFF_MAX_CONTACTS  5
#define                   rOFF_TRIGGER  6
#define                  rOFF_CHECKSUM  184
#define              rOFF_FRESH_CONFIG  185

// possible errors returned
#define             GT9XX_err_i2c_read  0xfe
#define            GT9XX_err_i2c_write  0xfd
#define               GT9XX_err_malloc  0xfc
#define   GT9XX_err_missing_rst_pullup  0xfb
#define        GT9XX_err_check_chip_id  0xfa
#define   GT9XX_err_check_cfg_checksum  0xf9
#define      GT9XX_err_check_cfg_fresh  0xf8
#define                 GT9XX_err_misc  EXIT_FAILURE

    struct firmware {
        size_t size;
        uint8_t *data;
    };

    struct GT9XX_point_t {
        // 0x814F-0x8156, ... 0x8176 (5 points) 
        uint8_t trackId;
        uint16_t x;
        uint16_t y;
        uint16_t area;
        uint8_t reserved;
    } __attribute__ ((packed));

    struct GT9XX_coord_t {
        uint8_t count;
        struct GT9XX_point_t point[GT9XX_COORD_MAX_COUNT-1];
    };

    struct goodix_ts_data {
        uint16_t usci_base_addr;
        uint8_t slave_addr;
        uint16_t id;
        struct GT9XX_coord_t coord;
        struct firmware conf;
    };



    uint8_t GT9XX_init(struct goodix_ts_data *t);

    uint8_t GT9XX_read(struct goodix_ts_data *t, const uint16_t reg,
                       uint8_t * buf, const size_t buf_len);
    uint8_t GT9XX_read_version(struct goodix_ts_data *t);
    int16_t GT9XX_read_state(struct goodix_ts_data *t, uint8_t * data);
    uint8_t GT9XX_read_config(struct goodix_ts_data *t);
    void GT9XX_free_config(struct goodix_ts_data *t);
    uint8_t GT9XX_write(struct goodix_ts_data *t, const uint16_t reg,
                        uint8_t * buf, const size_t buf_len);
    uint8_t GT9XX_write_config(struct goodix_ts_data *t);

    uint8_t GT9XX_clear_irq(struct goodix_ts_data *t);
    void GT9XX_disable_irq(void);
    void GT9XX_enable_irq(void);

    uint8_t GT9XX_check_chipid(const uint16_t id);

    void gt9xx_rst_event(void);
    uint8_t gt9xx_get_event(void);

    int16_t GT9xx_check_cfg_8(struct goodix_ts_data *t,
			const struct firmware *cfg);
    
    uint8_t GT9XX_calc_checksum(uint8_t* buf, uint16_t len);

    // callback function
    void (*GT9XX_HLHandler)(struct GT9XX_coord_t*);
    void GT9XX_set_HLHandler(void (*handler)(struct GT9XX_coord_t*));

    uint16_t _strtou16(char *buf);

#ifdef __cplusplus
}
#endif
#endif
