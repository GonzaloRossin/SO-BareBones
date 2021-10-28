#ifndef STRUCTS_H
#define STRUCTS_H
#include<stdint.h>
typedef struct screenShell
{
   char* buffer;
   int marginleft;
   int marginright;
   int buffersize;
   int coordX;
   int coordY; 
}screenShell;

typedef struct mm_stat {
    char * sys_name;  
    uint64_t total;
    uint64_t occupied;
    uint64_t free;
    uint64_t successful_allocs;
    uint64_t successful_frees;
} mm_stat;
typedef unsigned long size_t;
typedef void* address_t;
typedef int pid_t;
typedef enum {READY = 0, BLOCKED, KILLED} pStatus;

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

typedef unsigned int sem_id;

typedef struct sem_queue {
   int pid;
   struct sem_queue * next;
} sem_queue;

#define MAX_PROCESSES_PER_SEMAPHORE 10

typedef struct sem_t {
    char * name;
    int lock; 
    unsigned int value;
    sem_queue processes[MAX_PROCESSES_PER_SEMAPHORE];
    unsigned int processes_amount; //processes not closed 
    unsigned int processes_size; //all processes
    unsigned int processes_waiting; 
    sem_queue * first;
    sem_queue * last;
} sem_t;

typedef struct process_info {
    char name[50];
    void * rsp;
    void * rbp;
    int pid;
    int ppid;
    int foreground;
    unsigned int status;
    unsigned int priority;
    unsigned int given_time;
    unsigned int aging; 
} process_info;

#endif