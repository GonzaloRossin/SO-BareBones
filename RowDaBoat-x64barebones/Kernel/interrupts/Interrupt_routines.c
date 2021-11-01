// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "interrupt_routines.h"
#include <stdint.h>
#include "../drivers/keyboard_driver.h"
#include "../include/lib.h"
#include "../drivers/video_driver.h"
#include "../include/naiveConsole.h"
#include "interrupts.h"
#include "../include/process.h"

static unsigned long ticks = 0;
static unsigned long seconds=0;
static int is_shown=0;

uint64_t snapshotIP, snapshotSP;

static void pTimersHandler(void);
static int addTimer(int pid, unsigned int millis);


static process_waiting timers[MAX_PROCESSES];
static unsigned int processes_waiting_size = 0;

static char *regs[REG_SIZE] = {
	"R15: 0x", "R14: 0x", "R13: ", "R12: 0x", "R11: 0x", "R10: 0x", "R9: 0x",
	"R8: 0x", "RSI: 0x", "RDI: 0x", "RBP: 0x", "RDX: 0x", "RCX: 0x", "RBX: 0x",
	"RAX: 0x", "IP: 0x", "RSP: 0x"};

// called every 55ms. Increases the ticks counter
void * timerTickHandler(void * rsp)
{
	ticks++;	
	if(ticks % 18 == 0) {
		is_shown=show_cursor(is_shown);
		seconds++;
	}
	pTimersHandler();
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

static void pTimersHandler(void) {
	for (int i = 0; i < processes_waiting_size; i++) {
		timers[i].ticks_left--;
		if (timers[i].ticks_left <= 0) {
			changeStatus(timers[i].pid, READY);

			for (int j = i; j < processes_waiting_size - 1; j++)
				timers[j] = timers[j+1];
			processes_waiting_size--;
		}	
	}
}

static int addTimer(int pid, unsigned int millis) {
	
	unsigned int i = 0;
	while (i < processes_waiting_size) { //si ya tiene un timer que lo actualice
		if (timers[i].pid == pid) {
			timers[i].ticks_left = ( millis * PIT_FREQUENCY ) / 1000;
			return 0;
		}
		i++;
	}	
	if (i == MAX_PROCESSES)  // This should never happen
		return -1;
		
	timers[processes_waiting_size].pid = pid;
	timers[processes_waiting_size].ticks_left = ( millis * PIT_FREQUENCY ) / 1000;
	processes_waiting_size++;
	return 0;
}

void wait(unsigned int millis) {
	int pid;
	getPid(&pid);

	if (addTimer(pid, millis) == 0);
		changeStatus(pid, BLOCKED); // No corre mas.
}