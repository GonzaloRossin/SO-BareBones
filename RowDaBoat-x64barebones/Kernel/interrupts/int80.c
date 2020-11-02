#include <int80.h>
#include <video_driver.h>
#include <interrupt_routines.h>
#include <lib.h>
#include <keyboard_driver.h>
#include <stdint.h>
#include <font.h>

//Software interrupt used for interaction between user and kernel space
uint64_t int80Dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rax)
{

	switch (rdi)
	{
	//sys_read. Just reads from the keyboard and copies to the buffer.
	case 0:
		sys_read( (char *) rsi, (int)rdx);
		break;
	// sys_write: writes stdout the content of buffer.
	case 1:
		sys_write((char *) rsi, (int)rdx);
		break;
	case 3:
		sys_newline();
		break;
	case 4:
		sys_cpuInfo((char *) rsi, (uint32_t *) rdx);
		break;
	}
	return 0;
}

//SYSCALL 0
void sys_read ( char* rsi, int rdx){
	int i = 0;
	char letter;
	while (i < rdx)
	{
		letter = getChar();
		if (letter != 0)
		{
			*(rsi + i) = letter;
			i++;
		}
		else
		{
			//If there is nothing on the buffer we stop until we get an interruption.
			//haltFunction();
		}
	}
}

//SYSCALL 1
void sys_write( char * rsi, int rdx)
{
	draw_string(rsi,rdx);
}
//SYS_CALL 3
void sys_newline(){
	newLine();
}
//SYS_CALL 4
void sys_cpuInfo(char * vendor , uint32_t * version){
    cpuVendor(vendor);
    cpuVersion(version);
}	