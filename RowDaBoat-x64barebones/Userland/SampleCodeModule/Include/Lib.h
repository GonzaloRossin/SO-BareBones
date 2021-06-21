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

#endif