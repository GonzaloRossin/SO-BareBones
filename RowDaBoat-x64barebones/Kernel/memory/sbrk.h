#ifndef SBRK_H
#define SBRK_H
    
#include <stdint.h>

//  increments the end of DataSeg and return the beginning of the allocked space 
void sbrkHandler(int increment, void ** buffer);
#ifndef NULL
#define NULL ((void*) 0)
#endif

#endif