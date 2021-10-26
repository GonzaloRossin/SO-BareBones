#ifndef SBRK_H
#define SBRK_H
    
#include <stdint.h>

//  increments the end of DataSeg and return the beginning of the allocked space 
void sbrkHandler(int increment, void ** buffer);
#ifndef NULL
#define NULL ((void*) 0)
#endif
#define MIN_ADDRESS 0x600000
#define MAX_ADDRESS 0xF00000

#endif