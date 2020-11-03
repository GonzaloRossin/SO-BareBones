#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <int80.h>
#include <idt.h>
#include <video_driver.h>
#include <font.h>


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
	for (int i = 0; i < 1000; i++)
	{
		draw_string("hola", 4);
		draw_char(' ');
	}
	for (int i = 0; i < 300; i++)
	{
		draw_string("casa", 4);
		draw_char(' ');
	}
	draw_string("scrolled",8);
	newLine();
	draw_string("new line",8);
	clean();
	draw_string("cleaned",7);
	//Entering sampleCodeModuleAddress in userland
	//((EntryPoint)sampleCodeModuleAddress)();
	return 0;
}
