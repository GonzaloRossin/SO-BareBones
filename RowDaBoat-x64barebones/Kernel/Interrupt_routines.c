#include <interrupt_routines.h>
#include <stdint.h>
#include <naiveConsole.h>
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
// called when there is an 21h interrupt(keyboard interrupt)
void interruptRoutine2()
{
	uint8_t c = get_key(); //Get key
	ncPrint("hola ");
	//keyboard_handler(c);
}