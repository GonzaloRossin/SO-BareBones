#include <stdint.h>
#include "include/lib.h"
#include "interrupts/int80.h"
#include "include/moduleLoader.h"
#include <idt.h>
#include "drivers/video_driver.h"
#include "include/font.h"
#include <interrupt_routines.h>
#include "process.h"

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
	// ((EntryPoint)sampleCodeModuleAddress)();

	//start shell process, createProcess de process.c con datos hardcodeados
	//(char *name, int (*code)(int, char **), char **argv, size_t stack, size_t heap)
	char name[6] = "Shell";
	pCreate(name, (int (*)(int, char **)) sampleCodeModuleAddress, NULL, MAX_STACK, MAX_STACK); 
	return 0;
}
