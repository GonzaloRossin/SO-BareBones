#include <stdint.h>
#include "include/lib.h"
#include "interrupts/int80.h"
#include "include/moduleLoader.h"
#include "interrupts/idt.h"
#include "drivers/video_driver.h"
#include "include/font.h"
#include <interrupt_routines.h>
#include "include/process.h"
//#include "pipes/pipe.h"

#ifdef BUDDY_H
#include "../memory/buddy.h"
#else
#include "../memory/mem_man.h"
#endif

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

int main()
{	
	loadIDT();
	//Entering sampleCodeModuleAddress in userland
	initialStateSnapshot((uint64_t)sampleCodeModuleAddress, getSP());
	
	//start shell process, createProcess de process.c con datos hardcodeados
	//(char *name, int (*code)(int, char **), char **argv, size_t stack, size_t heap)
	main_func_t aux = {(int (*)(int, char **)) sampleCodeModuleAddress, 0, NULL};
	int pid;
	pCreate(&aux, "SampleCodeModule", 1, &pid);

	//initPipes();


/*
	#ifdef BUDDY_H
		draw_string("buddy",5);
	#else
		#ifdef MEM_MAN_H
			draw_string("mem man h",9);
		#endif
	#endif
	*/

	_halt_and_wait();

	// ((EntryPoint)sampleCodeModuleAddress)();
	return 0;
}
