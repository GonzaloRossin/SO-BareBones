#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <syscalls.h>
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
	int x = 0;
	int y = 0;
	char * word = "hola";

	for (int i = 0; i < 100; i++)
	{
		if (x >= SCREEN_WIDTH)
		{
			x = 0;
			y+=CHAR_HEIGHT+1;
		}
		
		print_word(x, y, word, 4, FONT_SIZE, FONT_COLOR, BACKGROUND_COLOR);
		x += 5*CHAR_WIDTH;
	}

	//((EntryPoint)sampleCodeModuleAddress)();

	return 0;
}
