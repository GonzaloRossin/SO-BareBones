#ifndef __INT80__
#define __INT80__
#include <stdint.h>
#include "../drivers/video_driver.h"
#include "interrupt_routines.h"
#include "../include/lib.h"
#include "../drivers/keyboard_driver.h"
#include "../include/process.h"
#include "../include/font.h"
#include "../memory/sbrk.h"
#include "../sem/sem.h"
#include "../pipes/pipe.h"

/*
#ifdef BUDDY_H
#include "../memory/buddy.h"
#else
#include "../memory/mem_man.h"
#endif
*/


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
void* MyMalloc(uint64_t rsi);// sys call 14
void MyFree(void* rsi);// sys call 15
void* memSet(void* rsi,uint32_t rdx,uint64_t rcx);//sys call 16
void mem(mm_stat*rsi);// sys call 17
int exec(main_func_t * rsi, char* rdx, int rcx, int * r8); //18
int ps(process_info * rsi, unsigned int rdx, unsigned int * rcx);// sys call 19
void process_kill(int rsi);// sys call 21
void nice(int rsi, unsigned int rdx);// sys call 22
void block(int rsi, unsigned int rdx);// sys call 23
uint64_t sys_sem(int rsi,uint64_t rdx,uint64_t rcx);//syscall 24
int get_process_status(int rsi, unsigned int *rdx);//syscall 25
void get_pid(int* rsi);//syscall26
void _wait(unsigned int millis);//syscall27
uint64_t pipe_open(char* name);//SYS_CALL 28
uint64_t pipe_close(uint64_t pipeId);//SYS_CALL 29
uint64_t read_pipe(uint64_t pipeId);//SYS_CALL 30
uint64_t write_pipe(uint64_t pipeId, char* string);//SYS_CALL 31
#endif