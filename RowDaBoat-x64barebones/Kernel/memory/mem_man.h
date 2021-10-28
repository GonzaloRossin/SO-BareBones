#ifdef MEM_MAN_H
    #include <stdio.h>
    #include <stdint.h>
    #include "sbrk.h"
    
    /* Define the linked list structure.  This is used to link free blocks in order
    of their memory address. */
    typedef struct A_BLOCK_LINK
    {
        struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
        uint64_t xBlockSize;						/*<< The size of the free block. */
    } BlockLink_t; //Free List

    /* Block sizes must not get too small. */
    #define heapMINIMUM_BLOCK_SIZE	( ( uint64_t ) ( xHeapStructSize << 1 ) )

    /* Assumes 8bit bytes! */
    #define heapBITS_PER_BYTE		( ( uint64_t ) 8 )

    /* 16 MB for the Heap in this case */
    #define configTOTAL_HEAP_SIZE ( (uint64_t) maxAddress - ( uint64_t ) minAddress )

    #define portBYTE_ALIGNMENT 64
    #define portBYTE_ALIGNMENT_MASK 0x3F

#endif