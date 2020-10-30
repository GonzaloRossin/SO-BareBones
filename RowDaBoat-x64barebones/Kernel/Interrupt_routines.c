#include <interrupt_routines.h>
#include <stdint.h>
#include <lib.h>

static unsigned long ticks = 0;
// called every 55ms. Increases the ticks counter
void interruptRoutine1()
{
	ticks++;
}

// returns the amount of ticks
unsigned long getTicks()
{
	return ticks;
}