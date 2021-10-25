#include <stdint.h>
#include "include/lib.h"
#include "interrupts/int80.h"
#include "include/moduleLoader.h"
#include "interrupts/idt.h"
#include "drivers/video_driver.h"
#include "include/font.h"
#include <interrupt_routines.h>
#include "include/process.h"
#include "memory/mem_man.h"
#include "memory/buddy.h"

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
	char name[6] = "Shell";
	main_func_t main_f;
	main_f.f = sampleCodeModuleAddress;
	main_f.argc = 0;
	main_f.argv = NULL;
	//pCreate(name, &main_f, MAX_STACK, MAX_STACK);

	//_halt_and_wait();
	
	//while(1){
	//	draw_string("hola", 5);
	//}
	initHeap();
	((EntryPoint)sampleCodeModuleAddress)();
	return 0;
}
