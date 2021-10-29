#ifndef LIB_H
#define LIB_H

#include "structs.h"
#include "libasm.h"



//-----------------------------------------------------------------------------------------------
//general use functions
int strlen(char *str);
int strcmp(const char *str1, const char *str2);
void strncpy(char *source, char *dest,int index, int size);
void numToChar(double num, char * c);
uint8_t * strToNumHex(char * str);
int abs(int i);
int strToInt(char* str);
void save_screenCords(screenShell* shell);
//-------------------------------------------------------------
//syscall
char read_input();//syscall0
void print(char *buffer);//syscall1
void get_Memory(uint8_t* mem,uint8_t* v);//syscall2
void newLine();//syscall3
void get_InfoReg(uint64_t* reg_pointer);//syscall4
void put_char(char c);//syscall5
void actionCall(int action);//syscall6
uint64_t get_RTC(uint64_t c);//syscall7
void print_num(int num, int base_option);//syscall8
void clearScreen();//syscall9
unsigned long get_seconds();//syscall10
void set_cursor(int x, int y);//syscall11
void set_margins(int mLeft,int mRight);//syscall12
void getCursor(int coords[2]);//syscall13
void* Mmalloc(int size);//syscall14
void Mfree(void* pointer);//syscall15
void* MemSet(void* ptr,uint32_t c,uint64_t string);//syscall16
pid_t exec(main_func_t *func, char* name, int rcx);//exec syscall18
mm_stat Mmem();//syscall17
int ps(process_info* arr, unsigned int max_size);//syscall19
void _yield();//syscall20
int kill(pid_t pid);//syscall21
void nice(pid_t pid, unsigned int priority);//syscall22
int block(pid_t pid, unsigned int status);//syscall23void s_init();//syscall24
uint64_t s_open(char* name,int value);
uint64_t s_wait(uint64_t s_id);
uint64_t s_post(uint64_t s_id);
uint64_t s_close(uint64_t s_id);
void list_sem();
int getProcessStatus(int pid, unsigned int * status);
int getPid(void);
void wait(unsigned int millis);
void sleep(int seg);
uint64_t p_open(char* name);
uint64_t p_close(uint64_t pipeIndex);
uint64_t p_read(uint64_t pipeIndex);
uint64_t p_write(uint64_t pipeIndex, char* string);
#endif
