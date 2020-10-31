#ifndef __INT80__
#define __INT80__
#include <stdint.h>

// the int 80h dispatcher
uint64_t int80Dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rax);

void sys_read( char * rsi, int rdx);			//SYS_CALL 0
void sys_write(char * rsi, int rdx);			    //SYS_CALL 1

#endif