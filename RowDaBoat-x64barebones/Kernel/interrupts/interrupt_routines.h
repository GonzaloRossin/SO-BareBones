#ifndef INTERRUPT_ROUTINES_H
#define INTERRUPT_ROUTINES_H
#include <stdint.h>
#include <naiveConsole.h>
#define REG_SIZE 17
#define PIT_FREQUENCY 18

typedef struct process_waiting {
	int pid;
	unsigned int ticks_left;
}process_waiting;

void initialStateSnapshot(uint64_t IP, uint64_t SP);
extern uint64_t getSP();
void returnToSnapshot(uint64_t *stackframe);
// timer interrupt(int 20h)
void * timerTickHandler(void * rsp);
// what the interrupt 2(int 21h)
void interruptRoutine2();
// asm code used for interruptRoutine2
uint8_t get_key();

unsigned long getSeconds();
void wait(unsigned int millis);


void printRegister(uint64_t *pri);

void printException(uint8_t exc, uint64_t *stackframe);
#endif