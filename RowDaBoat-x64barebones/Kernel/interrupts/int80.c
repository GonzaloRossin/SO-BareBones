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
		return (uint64_t) sys_read();
		break;
	// sys_write: writes stdout the content of buffer.
	case 1:
		sys_write((char *) rsi, (int)rdx);
		break;
	case 2:
		sys_getMemory((uint8_t) rsi, (uint8_t*) rdx);
		break;
	case 3:
		sys_newline();
		break;
	case 4:
		sys_cpuInfo((char *) rsi, (uint32_t *) rdx);
		break;
	case 5:
		sys_get_InfoReg((uint64_t*) rsi);
	case 6:
		sys_put_char((char) rsi);
		break;
	case 7:
		sys_action_call((int) rsi);
		break;
	}
	return 0;
}

//SYSCALL 0
char sys_read (){
	return getLastInput();
}

//SYSCALL 1
void sys_write( char * rsi, int rdx)
{
	draw_string(rsi,rdx);
}
//SYSCALL 2
void sys_getMemory(uint8_t rsi, uint8_t* rdx){
    saveMemory(rsi);
    getMemory(rdx);
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
//SYS_CALL 5
void sys_get_InfoReg(uint64_t* rsi){
	getRegs(rsi);
}
//SYS CALL 7
void sys_action_call(int rsi){
	switch(rsi){
		case 0:
			newLine();
	}
}