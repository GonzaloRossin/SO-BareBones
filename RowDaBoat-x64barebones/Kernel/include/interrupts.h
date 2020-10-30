#ifndef INTERRUPS_H_
#define INTERRUPS_H_

#include <idt.h>

void irq0Handler(void);
void int_80(void);

void _cli(void);

void _sti(void);

void picMasterMask(uint8_t mask);

void picSlaveMask(uint8_t mask);

#endif