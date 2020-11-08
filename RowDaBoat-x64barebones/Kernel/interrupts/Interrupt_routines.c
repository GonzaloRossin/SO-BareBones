#include <interrupt_routines.h>
#include <stdint.h>
#include <keyboard_driver.h>
#include <lib.h>
#include <interrupts.h>
#include <video_driver.h>

static unsigned long ticks = 0;
static unsigned long seconds=0;
// called every 55ms. Increases the ticks counter
void interruptRoutine1()
{
	ticks++;
	if(ticks%18==0)
		seconds++;
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

void printException(uint8_t exc)
{
	// print exception info
	switch (exc)
	{
	case 0:
		draw_string("Excepcion 0:division por cero",29);
		break;
	case 6:
		draw_string("Excepcion 6:operacion invalida",30);
	}
	newLine();
}

void printRegister(uint64_t pri)
{
	saveRegs();
	uint64_t v[16] = {0};
    getRegs(v);
	draw_string("pir: ",5);
	draw_hex(pri);
	newLine();
	char regs[16][7] = {"rax: ", "rbx: ", "rcx: ", "rdx: ", "rbp: ", "rdi: ", "rsi: ", "r8:  ", "r9:  ", "r10: ", "r11: ", "r12: ", "r13: ", "r14: ", "r15: ", "rsp: "};
	for(int i=0;i<16;i++){
		draw_string(regs[i],5);
		draw_hex(v[i]);
        newLine();
    }
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