#include "mem_man.h"
#include "int80.h"

/* Based on FreeRTOS Kernel V10.4.3
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS */

/* Inserts a block of memory to the list of free mem blocks.
 * Merges adjacent free zones into one. */
static void addBlockToFreeList(header * block);

/* Sets up the required heap on the first call to malloc */
static void initHeap();

/* The size of the structure placed at the beginning of each allocated
 *  memory block must be correctly byte aligned. */
static const size_t heapStructSize = (sizeof(header) + ((size_t) (BYTE_ALIGNMENT - 1))) & ~((size_t) BYTE_ALIGNMENT_MASK);

/* Block sizes must not get too small. */
#define heapMINIMUM_BLOCK_SIZE ((size_t) (heapStructSize << 1 ))

/* Links to the free list*/
static header start, * pEnd = NULL;

static size_t freeBytesRemaining = 0;

/* Total memory heap*/
void * totalMemory;
void *topAddress = NULL;

/* Gets set to the top bit of an size_t type.  When this bit in the size
 * member of an header structure is set then the block belongs to the
 * application.  When the bit is free the block is still part of the free heap
 * space. */
static size_t xBlockAllocatedBit = 0;



void * RTOSMalloc(size_t requestedSize) {
    header * currp, * prevp;
    void * returnp = NULL;

    if(pEnd == NULL){
        initHeap();
    }

    /* Checks that the requested block doesn't set the top bit
     * The top bit determines if the block is free or not. */
    if((requestedSize & xBlockAllocatedBit) == 0) {
        /* The wanted size is smaller than our block size.
         * We proceed to make it larger */
        if((requestedSize > 0) && ((requestedSize + heapStructSize) >  requestedSize)) {
            
            requestedSize += heapStructSize;
        
            /* Ensure that blocks are always aligned. */
            if((requestedSize & BYTE_ALIGNMENT_MASK) != 0x00) {
                /* Byte alignment required. Check for overflow. */
                if( (requestedSize + (BYTE_ALIGNMENT - (requestedSize & BYTE_ALIGNMENT_MASK)))  > requestedSize) {
                    requestedSize += (BYTE_ALIGNMENT - (requestedSize & BYTE_ALIGNMENT_MASK));
                } else {
                    requestedSize = 0;
                }  
            }
        }

        /* We have free space for the requested size */
        if((requestedSize > 0) && ( requestedSize <= freeBytesRemaining)) {
            /* Traverse the list from the start	(lowest address) block until
            * one of adequate size is found. */
            prevp = &start;
            currp = start.next;

            while((currp->size < requestedSize) && (currp->next != NULL)) {
                prevp = currp;
                currp = currp->next;
            }

            /* If the end marker was reached then a block of adequate size
             * was not found. */
            if(currp != pEnd) {
                /* Return the memory space pointed to - jumping over the
                 * header structure at its start. */
                returnp = (void *) (((uint8_t *) prevp->next) + heapStructSize);

                /* This block is being returned for use so must be taken out
                 * of the list of free blocks. */
                prevp->next = currp->next;

                /* If the block is larger than required it can be split into two. */
                if((currp->size - requestedSize) > heapMINIMUM_BLOCK_SIZE) {
                    /* This block is to be split into two.  Create a new
                     * block following the number of bytes requested. The void
                     * cast is used to prevent byte alignment warnings from the compiler. */
                    header* insertp = (void *) (((uint8_t *) currp) + requestedSize);

                    /* Calculate the sizes of two blocks split from the single block. */
                    insertp->size = currp->size - requestedSize;
                    currp->size = requestedSize;

                    /* Insert the new block into the list of free blocks. */
                    addBlockToFreeList(insertp);
                }

                freeBytesRemaining -= currp->size;

                /* The block is being returned - it is allocated and owned
                 * by the application and has no "next" block. */
                currp->size |= xBlockAllocatedBit;
                currp->next = NULL;
            }
        }
    }
    return returnp;
}


void sbrk_handler(int increment, void **buffer)
{
        if (topAddress == NULL)
        {
                topAddress = START_ADDRESS;
        }

        if ((topAddress + increment) <= END_ADDRESS)
        {

                *buffer = topAddress;
                topAddress += increment;
        }
        else
        {
                *buffer = NULL;
        }
        return;
}

static void initHeap() {
    
    header * pFirstFreeBlock;
    uint8_t * pAlignedHeap;
    size_t totalMemPointer;
    size_t totalHeapSize = TOTAL_HEAP_SIZE;

    sbrk_handler(TOTAL_HEAP_SIZE, &totalMemory);

    /* Ensure the heap starts on a correctly aligned boundary. */
    totalMemPointer = (size_t) totalMemory;
    
    // It checks the last bits and if they are not aligned then we align them
    if( ( totalMemPointer & BYTE_ALIGNMENT_MASK ) != 0 ) {
        totalMemPointer += ( BYTE_ALIGNMENT - 1 );
        totalMemPointer &= ~((size_t) BYTE_ALIGNMENT_MASK);
        totalHeapSize -= totalMemPointer - (size_t) totalMemory;
    }

    pAlignedHeap = (uint8_t *) totalMemPointer;

    /* start is used to hold a pointer to the first item in the list of free
     * blocks.  The void cast is used to prevent compiler warnings. */
    start.next = (void *) pAlignedHeap;
    start.size = (size_t) 0;

    /* pEnd is used to mark the end of the list of free blocks and is inserted
     * at the end of the heap space. */
    totalMemPointer = ((size_t) pAlignedHeap) + totalHeapSize;//last block(node+heapStruct)
    totalMemPointer -= heapStructSize;//node
    totalMemPointer &= ~((size_t) BYTE_ALIGNMENT_MASK);//allignment
    pEnd = (void *) totalMemPointer;
    pEnd->size = 0;
    pEnd->next = NULL;

    /* To start, there is a single free block that is sized to take up the
     * entire heap space, minus the space taken by pEnd. */
    pFirstFreeBlock = (void *) pAlignedHeap;
    pFirstFreeBlock->size = totalMemPointer - (size_t) pFirstFreeBlock;
    pFirstFreeBlock->next = pEnd;

    /* Only one block exists - and it covers the entire usable heap space. */
    freeBytesRemaining = pFirstFreeBlock->size;

    /* Work out the position of the top bit in a size_t variable. */
    xBlockAllocatedBit = ((size_t) 1) << ((sizeof(size_t) * heapBITS_PER_BYTE) - 1);
}



void RTOSFree(void *ptr){

    uint8_t * currp = (uint8_t *) ptr;
    header * insertp;

    if(ptr != NULL) {
        /* The memory being freed will have a header structure immediately before it. */
        currp -= heapStructSize;

        /* This casting is to keep the compiler from issuing warnings. */
        insertp = (void *) currp;

        if((insertp->size & xBlockAllocatedBit) != 0) {
            if(insertp->next == NULL) {
                /* The block is being returned to the heap - it is no longer allocated. */
                insertp->size &= ~xBlockAllocatedBit;
                
                /* Add this block to the list of free blocks. */
                freeBytesRemaining += insertp->size;
                // traceFREE( ptr, insertp->size );
                addBlockToFreeList(((header *) insertp));
            }
        }
    }
}



static void addBlockToFreeList(header * blockToInsert) {
    
    header * iteratorp;
    uint8_t * currp;

    /* Iterate through the list until a block is found that has a higher address
     * than the block being inserted. */
    iteratorp = &start;
    while(iteratorp->next < blockToInsert) {
        iteratorp = iteratorp->next;
    }

    /* Do the block being inserted, and the block it is being inserted after
     * make a contiguous block of memory? */
    currp = (uint8_t *) iteratorp;
    // unir del lado izquierdo 
    if((currp + iteratorp->size) == (uint8_t *) blockToInsert) {
        iteratorp->size += blockToInsert->size;
        blockToInsert = iteratorp;
    }

    /* Do the block being inserted, and the block it is being inserted before
     * make a contiguous block of memory? */
    currp = (uint8_t *) blockToInsert;
    
    // unir por derecha
    if((currp + blockToInsert->size) == (uint8_t *) iteratorp->next) {
        if(iteratorp->next != pEnd) {
            /* Form one big block from the two blocks. */
            blockToInsert->size += iteratorp->next->size;
            blockToInsert->next = iteratorp->next->next;
        } else {
            blockToInsert->next = pEnd;
        }
    } else {
        blockToInsert->next = iteratorp->next;
    }

    /* If the block being inserted plugged a gab, so was merged with the block
     * before and the block after, then it's next pointer will have
     * already been set, and should not be set here as that would make it point
     * to itself. */
    if(iteratorp != blockToInsert) {
        iteratorp->next = blockToInsert;
    }
}


void RTOSmem(){
    header *curr = &start;
    char *message1 = "Block: ";
    char *message2 = ", Bytes: ";
    for (int i = 1; curr != NULL; curr = curr->next, i++)
    {
        draw_string(message1, 7);
        sys_print_num(i, 0);
        draw_string(message2, 9);
        sys_print_num(curr->size, 0); //nose si está bien llamar syscalls desde acá
        sys_newline(); //newline() funciona pero es funcion del driver
    }
    sys_newline();
}