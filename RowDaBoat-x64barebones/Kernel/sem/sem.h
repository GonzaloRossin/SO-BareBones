#ifndef SEM_H
#define SEM_H

#define MAX_SEMAPHORES 10
#define MAX_PROCESSES_PER_SEMAPHORE 10

#include "../include/lib.h"
#include "../include/process.h"

typedef unsigned int sem_id;

typedef struct sem_queue {
   int pid;
   struct sem_queue * next;
} sem_queue;

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

sem_id sem_open(const char * name);
sem_id sem_init_open(const char * name, unsigned int init_val);
int sem_wait(sem_id sem);

#endif