#ifndef __INT80__
#define __INT80__
#include <stdint.h>

// the int 80h dispatcher
uint64_t int80Dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx,uint64_t r8);

char sys_read();			//sys call 0
void sys_write(char * rsi, int rdx);			//sys call 1
void sys_getMemory(uint8_t* rsi, uint8_t* rdx); //sys call 2
void sys_newline();                         //sys call 3
void sys_get_InfoReg(uint64_t* rsi);//sys call 4
void sys_put_char(char rsi); //sys call 5
void sys_action_call(int rsi);//sys call 6
uint8_t sys_get_clock(int rsi);//sys call 7
void sys_print_num(int rsi,int rdx);//sys call 8
void sys_clear_screen();//sys call 9
unsigned long sys_get_seconds();//sys call 10
void sys_cursor(int rsi, int rdx);//sys call 11
void sys_setMargins(int rsi,int rdx);//sys call 12
void sys_getCoords(int* rsi);// sys call 13
void* MyMalloc(unsigned int rsi);// sys call 14
void MyFree(void* rsi);// sys call 15
void* memSet(void* rsi,uint32_t rdx,uint64_t rcx);//sys call 16
void mem();// sys call 17
#endif