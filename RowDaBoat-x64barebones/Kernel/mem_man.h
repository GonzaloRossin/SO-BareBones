#ifndef MEM_MAN_H
#define MEM_MAN_H

    #include <stdio.h>
    #include <lib.h>
    #include <stdint.h>

    typedef struct Header {
    struct Header* next;
    size_t size;
    } header;

    #define BYTE_ALIGNMENT 64
    #define BYTE_ALIGNMENT_MASK ( 0x003f )
    #define TOTAL_HEAP_SIZE ((size_t) 10 * 1024)
    #define heapBITS_PER_BYTE ((size_t) 8)
    #define START_ADDRESS 0x0000000000600000

    void Free(void *ptr);
    void * Malloc(size_t requestedSize);

#endif