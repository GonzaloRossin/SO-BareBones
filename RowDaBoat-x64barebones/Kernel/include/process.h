#ifndef PROCESS_H
#define PROCESS_H

#include "lib.h"
#include "../drivers/video_driver.h"


#define MAX_PROCESSES 50
#define MAX_STACK 50000
#define HALTER_EXTRA_STACK_SPACE 20
#define STATE_SIZE 5
#define REGS_SIZE 15

#define MIN_PRIORITY 0
#define MAX_PRIORITY 6
#define BASE_PRIORITY ((MAX_PRIORITY - MIN_PRIORITY)/2)
#define MIN_TICKS 5
#define MAX_TICKS 10

#define PRIOR_SLOPE ((MAX_TICKS - MIN_TICKS)/(MIN_PRIORITY - MAX_PRIORITY)) //To calculate the quantum of each queue (quantum(priorirty) = m*p + b)
#define PRIOR_INDVAR (MAX_TICKS - MIN_PRIORITY * PRIOR_SLOPE)

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

typedef enum PSTATUS {READY = 0, BLOCKED, KILLED } pStatus;

typedef struct process_t {
    char name[MAX_NAME_LENGTH];
    int pid;
    int ppid;
    int foreground;    
    unsigned int status;    
    unsigned int given_time;
    unsigned int aging;
    unsigned int priority;
    void * stack;
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

typedef struct process_info {
    char name[MAX_NAME_LENGTH];
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

#define NULL ((void *)0)


void * scheduler(void * rsp);

int pCreate(main_func_t * f, char *name, int foreground, int * pid);
int kill(int pid);
int exit(void);
void getPid(int *pid);
int getProcessesAlive(unsigned int * amount);
int getProcessesInfo(process_info * arr, unsigned int max_size, unsigned int * size);
int changePriority(int pid, unsigned int new_priority);
int changeStatus(int pid, unsigned int new_status);
int getProcessStatus(int pid, unsigned int * status);
int changeForegroundStatus(int pid, unsigned int status);
int isCurrentForeground(void);
extern void forceTimer();
void yield();
void PS();

#endif