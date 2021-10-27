#ifndef SBRK_H
#define SBRK_H
    
#include "../include/lib.h"
    #define MIN_ADDRESS 0x600000
    #define MAX_ADDRESS 0xF00000

    typedef struct mm_stat {
        char * sys_name;  
        uint64_t total;
        uint64_t occupied;
        uint64_t free;
        uint64_t successful_allocs;
        uint64_t successful_frees;
    } mm_stat;

    void *RTOSMalloc( uint64_t size);
    void RTOSFree( void* ptr );
    void getMMStats(mm_stat*stats);

#endif