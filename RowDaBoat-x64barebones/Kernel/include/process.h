#ifndef PROCESS_H
#define PROCESS_H

#include "lib.h"

#define STATE_SIZE 5
#define REGS_SIZE 15
#define MAX_PROCESSES 50


typedef unsigned long size_t;
typedef void* address_t;
typedef int pid_t;
typedef enum { KILLED = 0, READY, BLOCKED } pStatus;


static const uint64_t MAX_STACK = 0x50000;

typedef struct {
    address_t base;
    size_t size;
} memoryBlock;

typedef struct{
    char** argv;
    int argc;
} argument;

typedef struct {
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
    int (*f)(int, char *);
    int argc;
    char * argv;
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

#define NULL ((void *)0)
#define ERROR -1
#define MEMORY_OVERFLOW -1
#define MAX_PROCESS_COUNT 5 

void * scheduler(void * rsp);

pid_t pCreate(char *name, main_func_t * f, size_t stack, size_t heap);
int exit(void);
int kill(size_t pid);
process_t* get_process_by_id(pid_t pid);
pStatus get_pStatus(pid_t pid);
pid_t set_pStatus(pid_t pid, pStatus status);
void free_process(pid_t pid);
static void prepareStack(int (*main)(int argc, char ** argv), int argc, char ** argv, void * rbp, void * rsp);



#endif