#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

void _sti();
void _cli();
// change master pic mask
// disables the bits in 1
// example: 0xFE enables only irq0(timer interrupt)
void picMasterMask(uint8_t mask);
// same but for slave pic
void picSlaveMask(uint8_t mask);

char *cpuVendor(char *result);
//handler of timer tick
void irq0Handler();
//handler of Kb interrupt
void irq1Handler();

//given a Kb interrupt, gets the key pressed code
uint8_t get_key();

void int80();

#endif