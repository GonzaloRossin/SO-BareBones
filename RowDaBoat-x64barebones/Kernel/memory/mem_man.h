#ifndef MEM_MAN_H
#define MEM_MAN_H

    #include <stdio.h>
    #include "../include/lib.h"
    #include <stdint.h>
    #include "sbrk.h"

    typedef struct Header {
    struct Header* next;
    size_t size;
    } header;

    #define BYTE_ALIGNMENT 64
    #define BYTE_ALIGNMENT_MASK ( 0x003f )
    #define TOTAL_HEAP_SIZE 10 * 1024
    #define heapBITS_PER_BYTE ((size_t) 8)
    void const* minAddress=(void*) MIN_ADDRESS;
    void const* maxAddress=(void*) MAX_ADDRESS;

    void RTOSFree(void *ptr);
    void * RTOSMalloc(size_t requestedSize);
    void RTOSmem();
    int * get_MemInfo();
    void initHeap();
#endif