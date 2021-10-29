#ifndef SEM_H
#define SEM_H
#include "../include/lib.h"
#include "defs.h"
#include "../memory/sbrk.h"
#include "../include/process.h"
#define MAX_NAME 15
#define MAX_SEM 30
#define TRUE 1
#define FALSE 0

typedef struct pNode
{
    struct pNode *next;
    uint64_t pid;
} processNode;

typedef struct
{
    char name[MAX_NAME];
    processNode *firstProcess; // primer proceso esperando (waiting) en la fila
    processNode *lastProcess;  // ultimo proceso esperando en la fila
    uint64_t lock;
    int value;
    uint64_t size;     // cantidad de procesos que usan el sem
    uint64_t sizeList; // cantidad de procesos bloqueados
} sem_t;

extern uint64_t _xchg(uint64_t *lock, int value);
extern void forceTimer();
void initSems();
uint64_t semOpen(char *name, uint64_t initValue);
uint64_t semClose(char *name);
// Locks a semaphore. If successful (the lock was acquired), sem_wait() and sem_trywait() will return 0.  Otherwise, -1 is returned and errno is set, and the state of the semaphore is unchanged.
uint64_t semWait(uint64_t semIndex);
// the value of the semaphore is incremented, and all threads which are waiting on the semaphore are awakened. If successful, sem_post() will return 0.  Otherwise, -1 is returned.
uint64_t semPost(uint64_t semIndex);
void listSem();
char *getSemName(uint64_t semIndex);
void printProcessesSem(uint64_t semIndex);

#endif