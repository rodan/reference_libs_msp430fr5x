/*
* Copyright (c) 2019, Petre Rodan
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met: 
* 
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer. 
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution. 
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

#define  GT9XX_EV_NULL 0
#define   GT9XX_EV_IRQ 0x1

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

/*
#define GT9XX_MAX_HEIGHT   4096
#define GT9XX_MAX_WIDTH    4096
#define GT9XX_INT_TRIGGER    1

#define GT9XX_CONFIG_MAX_LENGTH  240
#define GT9XX_CONFIG_911_LENGTH  186
#define GT9XX_CONFIG_967_LENGTH  228
*/

/* Register defines */
#define GT9XX_rCMD  0x8040
#define GT9XX_rCFG  0x8047
#define GT9XX_rDATA 0x8140

// Write only registers
#define GT9XX_rCOMMAND        0x8040
#define GT9XX_rESD_CHECK      0x8041
#define GT9XX_rPROXIMITY_EN   0x8042

// Read/write registers
// The version number of the configuration file
#define GT9XX_rCONFIG_DATA  0x8047
// X output maximum value (LSB 2 bytes)
#define GT9XX_rMAX_X        0x8048
// Y output maximum value (LSB 2 bytes)
#define GT9XX_rMAX_Y        0x804A
// Maximum number of output contacts: 1~5 (4 bit value 3:0, 7:4 is reserved)
#define GT9XX_rMAX_TOUCH    0x804C

// Module switch 1
// 7:6 Reserved, 5:4 Stretch rank, 3 X2Y, 2 SITO (Single sided ITO touch screen), 1:0 INT Trigger mode */
#define GT9XX_rMOD_SW1      0x804D
// Module switch 2
// 7:1 Reserved, 0 Touch key */
#define GT9XX_rMOD_SW2      0x804E

// Number of debuffs fingers press/release
#define GT9XX_rSHAKE_CNT    0x804F

#define GT9XX_rFILTER       0x8050

#define GT9XX_rLARGE_TOUCH  0x8051
#define GT9XX_rNOISE_REDUCTION  0x8052
#define GT9XX_rTOUCH_LVL    0x8053
#define GT9XX_rLEAVE_LVL    0x8054
#define GT9XX_rLPC          0x8055
#define GT9XX_rREFRESH_RATE 0x8056
#define GT9XX_rX_OUT_THRESH 0x8057
#define GT9XX_rY_OUT_THRESH 0x8058
#define GT9XX_rSPACE_TB     0x805B
#define GT9XX_rSPACE_LR     0x805C
#define GT9XX_rMINI_FILTER  0x805D
#define GT9XX_rCONFIG_UPDATED  0x8100

// ReadOnly registers (device and coordinates info)
// Product ID (LSB 4 bytes, GT9110: 0x06 0x00 0x00 0x09)
#define GT9XX_rID           0x8140
// Firmware version (LSB 2 bytes)
#define GT9XX_rFW_VER       0x8144

// Current output X resolution (LSB 2 bytes)
#define GT9XX_rX_RES       0x8146
// Current output Y resolution (LSB 2 bytes)
#define GT9XX_rY_RES       0x8148
// Module vendor ID
#define GT9XX_rVENDOR_ID   0x814A

#define GT9XX_rCOORD_ADDR  0x814E

// command status register 1 byte
#define GT9XX_rCMD_STATUS  0x81A8

/* Commands for REG_COMMAND */
//0: read coordinate state
#define GT9XX_wREAD         0x00
// 1: difference value original value
#define GT9XX_wDIFFVAL      0x01
// 2: software reset
#define GT9XX_wSOFTRESET    0x02
// 3: Baseline update
#define GT9XX_wBASEUPDATE   0x03
// 4: Benchmark calibration
#define GT9XX_wCALIBRATE    0x04
// 5: Off screen (send other invalid)
#define GT9XX_wSCREEN_OFF   0x05

/* When data needs to be sent, the host sends command 0x21 to GT9x,
 * enabling GT911 to enter "Approach mode" and work as a transmitting terminal */
#define GT9XX_wHOTKNOT_TX   0x21
#define GT9XX_wCONFIG_UPDATED  0xAA

#define RESOLUTION_LOC    1
#define MAX_CONTACTS_LOC  5
#define TRIGGER_LOC 6

    struct GTInfo {
        // 0x8140-0x814A 
        char productId[4];
        uint16_t fwId;
        uint16_t xResolution;
        uint16_t yResolution;
        uint8_t vendorId;
    } __attribute__ ((packed));

#define GT9XX_COORD_MAX_COUNT  10
#define GT9XX_POINT_STRUCT_SZ  8
    struct GTPoint {
        // 0x814F-0x8156, ... 0x8176 (5 points) 
        uint8_t trackId;
        uint16_t x;
        uint16_t y;
        uint16_t area;
        uint8_t reserved;
    } __attribute__ ((packed));

    struct GTLevelConfig {
        uint8_t touch;          // Threshold of touch grow out of nothing
        uint8_t leave;          // Threshold of touch decrease to nothing
    };

    struct GTStylusConfig {
        uint8_t txGain;
        uint8_t rxGain;
        uint8_t dumpShift;
        struct GTLevelConfig level;
        uint8_t control;        //Pen mode escape time out period (Unit: Sec)
    };

    struct GTFreqHoppingConfig {
        uint16_t hoppingBitFreq;
        uint8_t hoppingFactor;
    };

    struct GTKeyConfig {
        // Key position: 0-255 valid 
        // 0 means no touch, it means independent touch key when 4 of the keys are 8 multiples
        uint8_t pos1;
        uint8_t pos2;
        uint8_t pos3;
        uint8_t pos4;
        uint8_t area;
        struct GTLevelConfig level;
        uint8_t sens12;
        uint8_t sens34;
        uint8_t restrain;
    };

    struct GTConfig {
        // start at 0x8047
        uint8_t configVersion;
        uint16_t xResolution;
        uint16_t yResolution;
        // 0x804C
        uint8_t touchNumber;    // 3:0 Touch No.: 1~10

        // 7:6 Reserved, 5:4 Stretch rank, 3 X2Y, 2 Sito
        // 1:0 INT trig method: 00-rising, 01-falling, 02-low level, 03-high level enquiry
        uint8_t moduleSwitch1;
        uint8_t moduleSwitch2;  // bit0 TouchKey
        uint8_t shakeCount;     // 3:0 Finger shake count
        // 0x8050
        // 7:6 First filter, 5:0 Normal filter (filtering value of original coordinate window, coefficiency is 1)
        uint8_t filter;
        uint8_t largeTouch;
        uint8_t noiseReduction;
        struct GTLevelConfig screenLevel;

        uint8_t lowPowerControl;        // Time to low power consumption (0~15s)
        uint8_t refreshRate;    // Coordinate report rate (Cycle: 5+N ms)
        uint8_t xThreshold;     //res
        uint8_t yThreshold;     //res
        uint8_t xSpeedLimit;    //res
        uint8_t ySpeedLimit;    //res
        uint8_t vSpace;         // 4bit top/bottom  (coefficient 32)
        uint8_t hSpace;         // 4bit left/right 
        //0x805D-0x8061
        uint8_t stretchRate;    //Level of weak stretch (Strtch X/16 Pitch)
        uint8_t stretchR0;      // Interval 1 coefficient
        uint8_t stretchR1;      // Interval 2 coefficient
        uint8_t stretchR2;      // Interval 3 coefficient
        uint8_t stretchRM;      // All intervals base number

        uint8_t drvGroupANum;
        uint8_t drvGroupBNum;
        uint8_t sensorNum;
        uint8_t freqAFactor;
        uint8_t freqBFactor;
        // 0x8067
        uint16_t pannelBitFreq; //Baseband of Driver group A\B (1526HZ<baseband<14600Hz)
        uint16_t pannelSensorTime;      //res
        uint8_t pannelTxGain;
        uint8_t pannelRxGain;
        uint8_t pannelDumpShift;
        uint8_t drvFrameControl;
        // 0x806F - 0x8071
        uint8_t NC_2[3];        // datasheet for fw > 1040 (Rev.00 2014-08-04)  shows something else here
        struct GTStylusConfig stylusConfig;
        // 0x8078-0x8079
        uint8_t NC_3[2];
        uint8_t freqHoppingStart;       // Frequency hopping start frequency (Unit: 2KHz, 50 means 100KHz )
        uint8_t freqHoppingEnd; // Frequency hopping stop frequency (Unit: 2KHz, 150 means 300KHz )
        uint8_t noiseDetectTims;
        uint8_t hoppingFlag;
        uint8_t hoppingThreshold;

        uint8_t noiseThreshold;
        uint8_t NC_4[2];
        // 0x8082
        struct GTFreqHoppingConfig hoppingSegments[5];
        // 0x8091
        uint8_t NC_5[2];
        struct GTKeyConfig keys;
        // 0x809d
        uint8_t NC_6[1];
        uint8_t gestureLargeTouch;
        uint8_t NC_7[2];
        uint8_t hotknotNoiseMap;
        uint8_t linkThreshold;
        uint8_t pXYThreshold;
        uint8_t GHotDumpShift;
        uint8_t GHotRxGain;
        // 0x80a6
        uint8_t FreqGain0;
        uint8_t FreqGain1;
        uint8_t FreqGain2;
        uint8_t FreqGain3;
        uint8_t NC_8[9];
        uint8_t CombineDis;
        uint8_t SplitSet;
        uint8_t NC_9[2];
        uint8_t SensorCH[13];
        // 0x80c5
        uint8_t NC_10[15];
        // 0x80d5
        uint8_t DriverCH[25];
        uint8_t NC_11[15];
        // 0x80ff
        uint8_t checksum;
    } __attribute__ ((packed));

    uint8_t GT9XX_init(const uint16_t usci_base_addr, const uint8_t slave_addr);

    uint8_t GT9XX_read(const uint16_t usci_base_addr, const uint8_t slave_addr, const uint16_t reg,
                       uint8_t * buf, const size_t buf_len);
    uint8_t GT9XX_read_info(const uint16_t usci_base_addr, const uint8_t slave_addr);
    int16_t GT9XX_read_state(const uint16_t usci_base_addr, const uint8_t slave_addr,
                             uint8_t * data);
    uint8_t GT9XX_write(const uint16_t usci_base_addr, const uint8_t slave_addr, const uint16_t reg,
                        uint8_t * buf, const size_t buf_len);

    uint8_t GT9XX_clear_irq(const uint16_t usci_base_addr, const uint8_t slave_addr);
    void GT9XX_disable_irq(void);
    void GT9XX_enable_irq(void);

    void gt9xx_rst_event(void);
    uint8_t gt9xx_get_event(void);

#ifdef __cplusplus
}
#endif

#endif
