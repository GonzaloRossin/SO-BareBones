#ifndef INTERRUPT_ROUTINES_H
#define INTERRUPT_ROUTINES_H
#include <stdint.h>

// timer interrupt(int 20h)
void interruptRoutine1();
// what the interrupt 2(int 21h)
void interruptRoutine2();
// asm code used for interruptRoutine2
uint8_t get_key();

unsigned long getTicks();

void printRegister(uint8_t reg, uint64_t regData);

void printException(uint8_t exc);
#endif