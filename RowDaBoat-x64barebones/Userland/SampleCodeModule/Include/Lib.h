#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <libasm.h>

//structure used
typedef struct screenShell
{
   char* buffer;
   int marginleft;
   int marginright;
   int buffersize;
   int coordX;
   int coordY; 
}screenShell;
//-----------------------------------------------------------------------------------------------
//general use functions
int strlen(char *str);
int strcmp(const char *str1, const char *str2);
void strncpy(char *source, char *dest,int index, int size);
void numToChar(double num, char * c);
uint8_t * strToNumHex(char * str);
int abs(int i);
void save_screenCords(screenShell* shell);
void my_invalid_op();
//-------------------------------------------------------------
//syscall
char read_input();//syscall0
void print(char *buffer);//syscall1
void get_Memory(uint8_t* mem,uint8_t* v);//syscall2
void newLine();//syscall3
void get_CPUvendor(char* c,uint32_t* v);//syscall4
void get_InfoReg(uint64_t* reg_pointer);//syscall5
void put_char(char c);//syscall6
void actionCall(int action);//syscall7
uint64_t get_RTC(uint64_t c);//syscall8
void print_num(int num, int base_option);//syscall9
void clearScreen();//syscall10
unsigned long get_seconds();//syscall12
void set_cursor(int x, int y);//syscall13
void set_margins(int mLeft,int mRight);//syscall14
void getCursor(int coords[2]);//syscall15

#endif