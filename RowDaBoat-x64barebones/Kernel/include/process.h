#ifndef PROCESS_H
#define PROCESS_H

#include "lib.h"
#include "video_driver.h"

#define STATE_SIZE 5
#define REGS_SIZE 15
#define MAX_PROCESSES 50
#define MAX_STACK 50000

#define MIN_PRIORITY 0
#define MAX_PRIORITY 6
#define BASE_PRIORITY ((MAX_PRIORITY - MIN_PRIORITY)/2)

#define MIN_TICKS 10
#define MAX_TICKS 100

#define PRIOR_SLOPE ((MAX_TICKS - MIN_TICKS)/(MIN_PRIORITY - MAX_PRIORITY)) //To calculate the quantum of each queue (quantum(priorirty) = m*p + b)
#define PRIOR_INDVAR (MAX_TICKS - MIN_PRIORITY * PRIOR_SLOPE)


typedef unsigned long size_t;
typedef void* address_t;
typedef int pid_t;
typedef enum { KILLED = 0, READY, BLOCKED } pStatus;

typedef struct {
    uint64_t base;
    size_t size;
} memoryBlock;

typedef struct{
    char** argv;
    int argc;
} argument;

typedef struct process_t{
    pid_t pid;
    pid_t ppid;
    char *process_name;
    pStatus status;
    int foreground;
    unsigned int given_time;
    unsigned int aging;
    unsigned int priority;
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

#define NULL ((void *)0)
#define ERROR -1
#define MEMORY_OVERFLOW -1
#define MAX_PROCESS_COUNT 5 

void * scheduler(void * rsp);

pid_t pCreate(main_func_t * f, char *name, int foreground);
int exit(void);
int kill(size_t pid);

// working on these
int getPid(void);
unsigned int getProcessesAmount(void);
int changePriority(int pid, unsigned int new_priority);
int changeStatus(pid_t pid, pStatus new_status);
int changeForegroundStatus(int pid, unsigned int state);

//dont sure we need
process_t get_process_by_id(pid_t pid);
pStatus get_pStatus(pid_t pid);
void free_process(pid_t pid);

static void prepareStack(int (*main)(int argc, char ** argv), int argc, char ** argv, void * rbp, void * rsp);
static void initQuantums();
void PS();
void update_priority(pid_t pid, unsigned int priority);
void change_status(pid_t pid);


#endif