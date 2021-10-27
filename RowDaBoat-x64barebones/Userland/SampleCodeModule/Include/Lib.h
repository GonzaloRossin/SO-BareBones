#ifndef LIB_H
#define LIB_H

#include "structs.h"
#include <libasm.h>



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
void ps();//syscall19
//void loop();////syscall20
void kill(pid_t pid);//syscall21
void nice(pid_t pid, unsigned int priority);//syscall22
void block(pid_t pid);//syscall23
sem_id s_init(char* sname,unsigned int init_size);//syscall24
sem_id s_open(char* name);//syscall25
int s_wait(sem_id s_id);//syscall26
void argTest(int a1, int a2, int a3);//syscall99 de testing, despues se puede borrar

#endif