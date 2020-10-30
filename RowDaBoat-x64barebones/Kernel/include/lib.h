#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpuVendor(char *result);
//handler of timer tick
void irq0Handler();
//handler of Kb interrupt
void irq1Handler();

//given a Kb interrupt, gets the key pressed code
uint8_t get_key();

void int80();

#endif