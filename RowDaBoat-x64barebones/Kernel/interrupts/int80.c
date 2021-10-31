#include "int80.h"

//Software interrupt used for interaction between user and kernel space
uint64_t int80Dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx,uint64_t r8, uint64_t r9)
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
		return (uint64_t) MyMalloc(rsi);
		break;
	case 15:
		MyFree((void*)rsi);
		break;
	case 16:
		return (uint64_t) memSet((void*)rsi,(uint32_t)rdx,(uint64_t)rcx);
		break;
	case 17:
		mem((mm_stat*)rsi);
		break;
	case 18:
		return exec((main_func_t *) rsi, (char*) rdx, (int) rcx, (int *) r8, (int *)r9);
		break;
	case 19:
		ps((process_info *)rsi, (unsigned int) rdx, (unsigned int*) rcx);
		break;
	case 20:
		pYield();
	 	break;
	case 21:
		return process_kill((int)rsi);
		break;
	case 22:
		nice((int)rsi, (unsigned int)rdx);
		break;
	case 23:
		return block((int)rsi, (unsigned int) rdx);
		break;
	case 24:
		return (uint64_t)sys_sem((int)rsi, rdx,rcx);
		break;
	case 25:
		return get_process_status((int )rsi, (unsigned int *)rdx);
		break;
	case 26:
		get_pid((int*) rsi);
		break;
	case 27:
		_wait((unsigned int) rsi);
		break;
	case 28:
		return pipe_open((char*) rsi);
		break;
	case 29:
		return pipe_close((uint64_t) rsi);
		break;
	case 30:
		return read_pipe((uint64_t) rsi);
		break;
	case 31:
		return write_pipe((uint64_t) rsi,(char*)rdx);
		break;
	case 32:
		list_pipes();
		break;
	}
	return 0;
}
//SYSCALL 0
char sys_read (){
	return (char) getChar();
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
void* MyMalloc(uint64_t rsi){
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
void mem(mm_stat* rsi){
	getMMStats(rsi);
}
//SYS_CALL 18
int exec(main_func_t *rsi, char* rdx, int rcx, int * r8, int* r9){
	return pCreate((main_func_t *) rsi, (char *) rdx, (int)(uint64_t) rcx, (int *) r8, (int *) r9);
}
//SYS_CALL 19
int ps(process_info *rsi, unsigned int rdx, unsigned int* rcx) {
	return getProcessesInfo((process_info *) rsi, (unsigned int) rdx, (unsigned int *) rcx);
}
// //SYS_CALL 20
void pYield(){
	yield();
}
// //SYS_CALL 21
int process_kill(int pid){
	return kill(pid);
}
//SYS_CALL 22
void nice(int pid, unsigned int priority) {
	changePriority(pid, priority);
}
// //SYS_CALL 23
int block(int pid, unsigned int new_status){
	return changeStatus(pid, new_status);
}
//SYS_CALL 24
uint64_t sys_sem(int rsi,uint64_t rdx,uint64_t rcx){
	switch (rsi)
	{
	case 0:
		initSems();
		break;
	case 1:
		return semOpen((char*) rdx,rcx);
	break;
	case 2:
		return semWait(rdx);
	break;
	case 3:
		return semPost(rdx);
	break;
	case 4:
		return semClose((char*)rdx);
	break;
	case 5:
		listSem();
	break;
	}
	return -2;
}
//SYS_CALL 25
int get_process_status(int pid, unsigned int *status) {
	return getProcessStatus((int) pid, (unsigned int *) status);
}
//SYS_CALL 26
void get_pid(int* pid){
	getPid((int *) pid);
}

//SYS_CALL 27
void _wait(unsigned int millis) {
	wait(millis);
}


//SYS_CALL 28
uint64_t pipe_open(char* name){
	return pipeOpen((char*) name);
}

//SYS_CALL 29
uint64_t pipe_close(uint64_t pipeId){
	return pipeClose((uint64_t) pipeId);
}

//SYS_CALL 30
char read_pipe(uint64_t pipeId){
	return (char)readPipe((uint64_t) pipeId);
}

//SYS_CALL 31
uint64_t write_pipe(uint64_t pipeId, char* string){
	return writePipe((uint64_t) pipeId,(char*)string);
}

//SYS_CALL 32
void list_pipes(){
	pipe();
}
