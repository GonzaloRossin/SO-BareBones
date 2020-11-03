#include <interrupt_routines.h>
#include <stdint.h>
#include <keyboard_driver.h>
#include <lib.h>
#include <interrupts.h>
#include <video_driver.h>

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
	keyboard_handler(c);
}

void printException(uint8_t exc)
{
	// print exception info
	switch (exc)
	{
	case 0:
		draw_string("Excepcion 0:division por cero",29);
		break;
	}
	newLine();
}

void printRegister(uint8_t reg, uint64_t regData)
{
	// get which register
	switch (reg)
	{
	case 0:
		draw_string("RSI: ",5);
		break;
	case 1:
		draw_string("RAX: ",5);
		break;
	case 2:
		draw_string("RBX: ",5);
		break;
	case 3:
		draw_string("RCX: ",5);
		break;
	case 4:
		draw_string("RDX: ",5);
		break;
	case 5:
		draw_string("RSP: ",5);
		break;
	case 6:
		draw_string("RBP: ",5);
		break;
	}
	draw_hex(regData);
	newLine();
}
void loader();
void reboot()
{
	while (getChar() != 0)
	{
		
	}
	newLine();
	draw_string("Press any key to exit",29);
	while (1)
	{
		if (getChar() != 0)
		{
			reboot_asm();
		}
	}
	reboot_asm();
}