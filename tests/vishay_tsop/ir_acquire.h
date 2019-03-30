#ifndef __IR_ACQUIRE_H__
#define __IR_ACQUIRE_H__

// how many times an IR signal can rise and fall during one command
#define    IR_ACQUIRE_MAX_DELTAS  256

// maximum interval for one command (in milliseconds)
#define  IR_ACQUIRE_MAX_INTERVAL  100

// tsop pin
#define                  IR_IES  P1IES
#define                  IR_IFG  P1IFG
#define                   IR_IE  P1IE
#define                   IR_IN  P1IN
#define                 IR_TRIG  BIT2

// state machine states

#define       IR_ACQUIRE_SM_IDLE  0x00
#define      IR_ACQUIRE_SM_START  0x01
#define    IR_ACQUIRE_SM_ONGOING  0x02
#define       IR_ACQUIRE_SM_STOP  0x04


struct ir_tome {
    uint16_t id;           // internal identifier for the current command
    uint16_t delta_size;   // count of elements in *delta
    uint16_t *delta;       // time intervals for each level in one command
    struct ir_tome * next; // next command in the tome
};


#ifdef __cplusplus
extern "C" {
#endif

void ir_acquire_sm_set_state(const uint16_t state);
void ir_acquire_start(void);
void ir_acquire_sm(void);
int ir_acquire_get_aquisition(uint16_t **data, uint16_t *size);

#ifdef __cplusplus
}
#endif

#endif
