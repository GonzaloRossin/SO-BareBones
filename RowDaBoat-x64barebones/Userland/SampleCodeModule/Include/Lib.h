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


typedef unsigned long size_t;
typedef void* address_t;
typedef int pid_t;
typedef enum { KILLED = 0, READY, BLOCKED } pStatus;

typedef struct {
    address_t base;
    size_t size;
} memoryBlock;

typedef struct{
    char** argv;
    int argc;
} argument;

typedef struct process_t{
    pid_t pid;
    char *process_name;
    pStatus status;
    unsigned int given_time;
    unsigned int aging;
    unsigned int priority;
    memoryBlock heap;
    memoryBlock stack;
    void * rbp;
    void * rsp;
    struct process_t * next_in_queue;    
} process_t;

typedef struct QUEUE_HD {
    unsigned int quantum;
    struct process_t * first;
    struct process_t * last;
} QUEUE_HD;

typedef struct main_func_t {
    int (*f)(int, char **);
    int argc;
    char ** argv;
} main_func_t;

typedef struct stackProcess {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} stackProcess;


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
void Mmem();//syscall17
//exec syscall18
void ps();//syscall19
//void loop();////syscall20
void kill(pid_t pid);//syscall21
void nice(pid_t pid, unsigned int priority);//syscall22
void block(pid_t pid);//syscall23
int* get_Minfo();//syscall24
void argTest(int a1, int a2, int a3);//syscall99 de testing, despues se puede borrar

#endif