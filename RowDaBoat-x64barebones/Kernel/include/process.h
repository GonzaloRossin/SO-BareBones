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