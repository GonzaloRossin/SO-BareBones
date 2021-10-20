#ifndef PROCESS_H
#define PROCESS_H

#include "lib.h"

typedef unsigned long size_t;
typedef void* address_t;
typedef int pid_t;
typedef enum { ZOMBIE = 0, ACTIVE, SUSPENDED } pStatus;


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
    address_t code_pos;
    argument arguments;
    memoryBlock heap;
    memoryBlock stack;
    
} process_t;

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


pid_t pCreate(char *name, address_t code, char **argv, size_t stack, size_t heap);
process_t* get_process_by_id(pid_t pid);
pStatus get_pStatus(pid_t pid);
pid_t set_pStatus(pid_t pid, pStatus status);
void free_process(pid_t pid);



#endif