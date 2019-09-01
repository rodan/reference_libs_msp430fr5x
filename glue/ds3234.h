#ifndef __DS3234_H_
#define __DS3234_H_

//#include "config.h"

#define SECONDS_FROM_1970_TO_2000 946684800

// control register bits
#define DS3234_A1IE     0x1
#define DS3234_A2IE     0x2
#define DS3234_INTCN    0x4

// status register bits
#define DS3234_A1F      0x1
#define DS3234_A2F      0x2
#define DS3234_OSF      0x80

void DS3234_init(const uint16_t baseAddress);
void DS3234_port_init(void);
void DS3234_set(const uint16_t baseAddress, struct ts t);
void DS3234_get(const uint16_t baseAddress, struct ts *t);

void DS3234_set_addr(const uint8_t pin, const uint8_t addr, const uint8_t val);
uint8_t DS3234_get_addr(const uint8_t pin, const uint8_t addr);

// control/status register
void DS3234_set_creg(const uint8_t pin, const uint8_t val);
void DS3234_set_sreg(const uint8_t pin, const uint8_t mask);
uint8_t DS3234_get_sreg(const uint8_t pin);

// aging offset register
void DS3234_set_aging(const uint8_t pin, const int8_t value);
int8_t DS3234_get_aging(const uint8_t pin);

// temperature register
float DS3234_get_treg(const uint8_t pin);

// alarms
void DS3234_set_a1(const uint8_t pin, const uint8_t s, const uint8_t mi, const uint8_t h, const uint8_t d,
                   const uint8_t * flags);
void DS3234_get_a1(const uint8_t pin, char *buf, const uint8_t len);
void DS3234_clear_a1f(const uint8_t pin);
uint8_t DS3234_triggered_a1(const uint8_t pin);

void DS3234_set_a2(const uint8_t pin, const uint8_t mi, const uint8_t h, const uint8_t d,
                   const uint8_t * flags);
void DS3234_get_a2(const uint8_t pin, char *buf, const uint8_t len);
void DS3234_clear_a2f(const uint8_t pin);
uint8_t DS3234_triggered_a2(const uint8_t pin);

// sram
void DS3234_set_sram_8b(const uint8_t pin, const uint8_t address, const uint8_t value);
uint8_t DS3234_get_sram_8b(const uint8_t pin, const uint8_t address);

#endif
