#ifndef BUDDY_H
#define BUDDY_H

#include <stdint.h>
#include <string.h>
#include "sbrk.h"

    #define BASE 2
    #define GIGA 30
    #define MIN_ALLOC_LOG2 6                            // Min size = 64 bytes
    #define MAX_ALLOC_LOG2 (GIGA - MIN_ALLOC_LOG2)      // Max size = 1 GB
    #define BINARY_POWER(x) (1 << (x))
    #ifndef TOTAL_HEAP_SIZE
    #define TOTAL_HEAP_SIZE 1024 * 10
    #endif

    #ifndef NULL
        #define NULL ((void*)0)
    #endif

    typedef enum { false, true } bool;

    typedef struct block_t
    {
        bool inUse;
        unsigned short int level;
        struct block_t* prev;
        struct block_t *next;
    } block_t;
    void RTOSFree(void *ptr);
    void *RTOSMalloc(size_t requestedSize);
    int* get_MemInfo();
    void initHeap();

#endif