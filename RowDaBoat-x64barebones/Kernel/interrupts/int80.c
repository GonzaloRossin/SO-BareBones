#include "int80.h"

//Software interrupt used for interaction between user and kernel space
uint64_t int80Dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx,uint64_t r8)
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
		sys_getMemory((uint8_t*) rsi, (uint8_t*) rdx);
		break;
	case 3:
		sys_newline();
		break;
	case 4:
		sys_get_InfoReg((uint64_t*) rsi);
		break;
	case 5:
		sys_put_char((char) rsi);
		break;
	case 6:
		sys_action_call((int) rsi);
		break;
	case 7:
		return sys_get_clock((int) rsi);
		break;
	case 8:
		sys_print_num((int)rsi,(int)rdx);
		break;
	case 9:
		sys_clear_screen();
		break;
	case 10:
		return sys_get_seconds();
	case 11:
		sys_cursor((int) rsi, (int) rdx);
		break;
	case 12:
		sys_setMargins((int)rsi,(int)rdx);
		break;
	case 13:
		sys_getCoords((int*)rsi);
		break;
	case 14:
		return (uint64_t) MyMalloc((unsigned int) rsi);
		break;
	case 15:
		MyFree((void*)rsi);
		break;
	case 16:
		return (uint64_t) memSet((void*)rsi,(uint32_t)rdx,(uint64_t)rcx);
		break;
	case 17:
		mem();
		break;
	case 18:
		return (pid_t) exec((main_func_t *) rsi, (char*) rdx, (int) rcx);
		break;
	case 19:
		ps();
		break;
	// case 20:
	// 	//loop();
	// 	break;
	// case 21:
	// 	process_kill((pid_t)rsi);
	// 	break;
	// case 22:
	// 	nice((pid_t)rsi, (unsigned int)rdx);
	// 	break;
	// case 23:
	// 	block((pid_t)rsi);
	// 	break;
	// case 24:
	// 	return (uint64_t) get_MemInfo();
	// 	break;
	 case 99:
	 	arg_test(rsi,rdx,rcx);
	 	break;
	}
	return 0;
}
//SYSCALL 99
void arg_test(int a1, int a2, int a3){
	char* msg = "Argumentos: ";
	draw_string(msg, 12);
	sys_newline();
	draw_decimal(a1);
	sys_newline();
	draw_decimal(a2);
	sys_newline();
	draw_decimal(a3);
	sys_newline();
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
void sys_get_InfoReg(uint64_t* rsi){
	getRegs(rsi);
}
//SYS_CALL 5
void sys_put_char(char rsi){
	draw_char(rsi);
}
//SYS CALL 6
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
//SYS_CALL 7
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
//SYS_CALL 8
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
//SYS_CALL 9
void sys_clear_screen(){
	clean();
}
//SYS_CALL 10
unsigned long sys_get_seconds(){
	return getSeconds();
}

//SYS_CALL 11
void sys_cursor(int rsi, int rdx){
	if (rsi > 0 && rdx > 0 && rsi < SCREEN_WIDTH && rdx < SCREEN_HEIGHT)
	{
		set_cursor(rsi, rdx);
	}
	else if (rsi==-1 && rdx ==-1)
	{
		return_last_cursor();
	}
}
//SYS_CALL 12
void sys_setMargins(int rsi,int rdx){
	set_margins(rsi,rdx);
}
//SYS_CALL 13
void sys_getCoords(int* rsi){
	getCoords(rsi);
}
//SYS_CALL 14
void* MyMalloc(unsigned int rsi){
	return (void*)RTOSMalloc(rsi);
}
//SYS_CALL 15
void MyFree(void* rsi){
	RTOSFree(rsi);
}
//SYS_CALL 16
void* memSet(void* rsi,uint32_t rdx,uint64_t rcx){
	return (void*)memset(rsi, rdx, rcx);
}
//SYS_CALL 17
void mem(){
	RTOSmem();
}
//SYS_CALL 18
pid_t exec(main_func_t *rsi, char* rdx, int rcx){
	return pCreate(rsi, rdx, rcx);
}
//SYS_CALL 19
void ps(){
	PS();
}
// //SYS_CALL 20
// //SYS_CALL 21
// void process_kill(pid_t pid){
// 	kill(pid);
// }
// //SYS_CALL 22
// void nice(pid_t pid, unsigned int priority){
// 	update_priority(pid, priority);
// }
// //SYS_CALL 23
// void block(pid_t pid){
// 	change_status(pid);
// }

