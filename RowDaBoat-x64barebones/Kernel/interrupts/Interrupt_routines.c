#include <interrupt_routines.h>
#include <stdint.h>
#include <keyboard_driver.h>
#include <lib.h>
#include <interrupts.h>
#include <video_driver.h>
#include <naiveConsole.h>
#include <process.h>

static unsigned long ticks = 0;
static unsigned long seconds=0;

uint64_t snapshotIP, snapshotSP;

static char *regs[REG_SIZE] = {
	"R15: 0x", "R14: 0x", "R13: ", "R12: 0x", "R11: 0x", "R10: 0x", "R9: 0x",
	"R8: 0x", "RSI: 0x", "RDI: 0x", "RBP: 0x", "RDX: 0x", "RCX: 0x", "RBX: 0x",
	"RAX: 0x", "IP: 0x", "RSP: 0x"};

// called every 55ms. Increases the ticks counter
void * timerTickHandler(void * rsp)
{
	ticks++;	
	if(ticks % 18 == 0) {
		seconds++;
	}
	return scheduler(rsp);
}

// returns the amount of ticks
unsigned long getSeconds()
{
	return seconds;
}
// called when there is an 21h interrupt(keyboard interrupt)
void interruptRoutine2()
{
	uint8_t c = get_key(); //Get key
	keyboard_handler(c);
}

void initialStateSnapshot(uint64_t IP, uint64_t SP)
{
	snapshotIP = IP;
	snapshotSP = SP;
}

void printException(uint8_t exc, uint64_t *stackframe)
{
	// print exception info
	switch (exc)
	{
	case 0:
		draw_string("Excepcion 0:division por cero",29);
		break;
	case 6:
		draw_string("Excepcion 6:operacion invalida",30);
		break;
	default: draw_string("default", 7);
	}
	newLine();
	printRegister(stackframe);
	clean_all();
	returnToSnapshot(stackframe);
}
void returnToSnapshot(uint64_t *stackframe)
{
	stackframe[REG_SIZE - 2] = snapshotIP; // RIP
	stackframe[REG_SIZE + 1] = snapshotSP; // RSP
}

void printRegister(uint64_t *pri)
{
	char toPrint[200];
	for (int i = 0; i < REG_SIZE; i++)
	{
		draw_string(regs[i], 7);
		uintToBase(pri[i], toPrint, 16);
		draw_string(toPrint, 5);
		newLine();
	}
	draw_string("Press left shift to exit",24);
	while(1) {
		if (get_key() == 0x2A) //left shift code
		{
			break;
		}	
	}
}