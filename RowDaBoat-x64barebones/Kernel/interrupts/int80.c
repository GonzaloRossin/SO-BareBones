#include <int80.h>
#include <video_driver.h>
#include <interrupt_routines.h>
#include <lib.h>
#include <keyboard_driver.h>
#include <stdint.h>
#include <font.h>

//Software interrupt used for interaction between user and kernel space
uint64_t int80Dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx)
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
		break;
	case 6:
		sys_put_char((char) rsi);
		break;
	case 7:
		sys_action_call((int) rsi);
		break;
	case 8:
		return sys_get_clock((int) rsi);
		break;
	case 9:
		sys_print_num((int)rsi,(int)rdx);
		break;
	case 10:
		sys_clear_screen();
		break;
	case 11:
		sys_draw((int) rsi, (matrix_struct *) rdx);
		break;
	case 12:
		return sys_get_seconds();
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
void sys_getMemory(uint8_t* rsi, uint8_t* rdx){
    saveMemory(rsi);
    getMemory(rdx);
}
//SYS_CALL 3
void sys_newline(){
	newLine();
}
//SYS_CALL 4
void sys_cpuInfo(char* rsi , uint32_t* rdx){
    cpuVendor(rsi);
    cpuVersion(rdx);
}
//SYS_CALL 5
void sys_get_InfoReg(uint64_t* rsi){
	getRegs(rsi);
}
//SYS_CALL 6
void sys_put_char(char rsi){
	draw_char(rsi);
}
//SYS CALL 7
void sys_action_call(int rsi){
	switch(rsi){
		case 0:
			newLine();
			break;
		case 1:
			delete_char();
			break;
		case 3:
			delete_line();
			break;
	}
}
//SYS_CALL 8
uint8_t sys_get_clock(int rsi){
	switch(rsi){
		case 0:
			return readSeconds();
		case 1:
			return readMinutes();
		case 2:
			return readHours();	
	}

	return 0;

}
//SYS_CALL 9
void sys_print_num(int rsi,int rdx){
	switch (rdx)
	{
	case 0:
		draw_decimal(rsi);
		break;
	case 1:
		draw_hex(rsi);
		break;
	}
}
//SYS_CALL 10
void sys_clear_screen(){
	clean();
}
//SYS_CALL 11
void sys_draw(int rsi, matrix_struct * rdx){
	switch (rsi)
	{
	case 0:
		draw_matrix(rdx->x, rdx->y, rdx->matrix, rdx->width, rdx->height, rdx->draw_size, rdx->color, rdx->backgroundcolor);
		break;
	case 1:
		draw_char_personalized(rdx->x, rdx->y, rdx->caracter, rdx->draw_size, rdx->color, rdx->backgroundcolor);
		break;
	}
}
unsigned long sys_get_seconds(){
	return getSeconds();
}


