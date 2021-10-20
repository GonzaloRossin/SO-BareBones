#ifndef INTERRUPTS_H
#define INTERRUPT_H
#include <stdint.h>


//handler of timer tick
void irq0Handler(void);
void * int_20(void * rsp);
//handler of Kb interrupt
void irq1Handler();

//given a Kb interrupt, gets the key pressed code
uint8_t get_key();

void int_80();



void _cli(void);

void _sti(void);

//Halt function
void Halt();

void picMasterMask(uint8_t mask);

void picSlaveMask(uint8_t mask);

//exception 0, division by 0
void exception0();
//exception 6, invalid operation
void exception6();
#endif