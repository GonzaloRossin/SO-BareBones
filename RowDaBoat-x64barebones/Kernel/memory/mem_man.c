#include "mem_man.h"
#include "../interrupts/int80.h"
#include "sbrk.h"
#include "../drivers/video_driver.h"

/* Based on FreeRTOS Kernel V10.4.3
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS */

/* Inserts a block of memory to the list of free mem blocks.
 * Merges adjacent free zones into one. */
static void addBlockToFreeList(header * block);

/* Sets up the required heap on the first call to malloc */
void initHeap();

/* The size of the structure placed at the beginning of each allocated
 *  memory block must be correctly byte aligned. */
static const size_t heapStructSize = (sizeof(header) + ((size_t) (BYTE_ALIGNMENT - 1))) & ~((size_t) BYTE_ALIGNMENT_MASK);

/* Block sizes must not get too small. */
#define heapMINIMUM_BLOCK_SIZE ((size_t) (heapStructSize << 1 ))

/* Links to the free list*/
static header start, * pEnd = NULL;

static uint64_t xTotalSpace = 0U;
static uint64_t xFreeBytesRemaining = 0U;
static uint64_t xMinimumEverFreeBytesRemaining = 0U;
int info[2] = {0};


/* Gets set to the top bit of an size_t type.  When this bit in the size
 * member of an header structure is set then the block belongs to the
 * application.  When the bit is free the block is still part of the free heap
 * space. */
static size_t xBlockAllocatedBit = 0;



void * RTOSMalloc(size_t requestedSize) {
    header*pxBlock, *pxPreviousBlock, *pxNewBlockLink;
    void *pvReturn = NULL;

		/* If this is the first call to malloc then the heap will require
		initialization to setup the list of free blocks. */
		if( pEnd == NULL )
			initHeap();

		/* Check the requested block size is not so large that the top bit is
		set.  The top bit of the block size member of the BlockLink_t structure
		is used to determine who owns the block - the application or the
		kernel, so it must be free. */
		if( ( requestedSize & xBlockAllocatedBit ) == 0 ) {//entra cuando wanted size es < 2 a la n-1
 
			/* The wanted size is increased so it can contain a BlockLink_t
			structure in addition to the requested amount of bytes. */
			if( requestedSize > 0 ) {

				requestedSize += heapStructSize;

				/* Ensure that blocks are always aligned to the required number
				of bytes. */
				if( ( requestedSize & BYTE_ALIGNMENT_MASK ) != 0x00 ) {
					/* Byte alignment required. */
					requestedSize += ( BYTE_ALIGNMENT - ( requestedSize & BYTE_ALIGNMENT_MASK ) );
				}
			}
		 

			if( ( requestedSize > 0 ) && ( requestedSize <= xFreeBytesRemaining ) ) {
				/* Traverse the list from the start	(lowest address) block until
				one	of adequate size is found. */
				pxPreviousBlock = &start;
				pxBlock = start.next;
				while( ( pxBlock->size < requestedSize ) && ( pxBlock->next != NULL ) ) {//CORTA SI ENCUENTRA O BLOCK O SI LLEGA LA FINAL
					pxPreviousBlock = pxBlock;
					pxBlock = pxBlock->next;
				}

				/* If the end marker was reached then a block of adequate size
				was	not found. */
				if( pxBlock != pEnd ) {//ENCONTRASTE UN BLOQUE

					/* Return the memory space pointed to - jumping over the
					BlockLink_t structure at its start. */
					pvReturn = ( void * ) ( ( ( uint8_t * ) pxPreviousBlock->next ) + heapStructSize );

					/* This block is being returned for use so must be taken out
					of the list of free blocks. */
					pxPreviousBlock->next = pxBlock->next;

					/* If the block is larger than required it can be split into
					two. */
					if( ( pxBlock->size - requestedSize ) > heapMINIMUM_BLOCK_SIZE ) {

						/* This block is to be split into two.  Create a new
						block following the number of bytes requested. The void
						cast is used to prevent byte alignment warnings from the
						compiler. */
						pxNewBlockLink = ( void * ) ( ( ( uint8_t * ) pxBlock ) + requestedSize );
						
						/* Calculate the sizes of two blocks split from the
						single block. */
						pxNewBlockLink->size = pxBlock->size - requestedSize;
						pxBlock->size = requestedSize;

						/* Insert the new block into the list of free blocks. */
						addBlockToFreeList( pxNewBlockLink );
					}

					xFreeBytesRemaining -= pxBlock->size;

					if( xFreeBytesRemaining < xMinimumEverFreeBytesRemaining ) {
						xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
					}

					/* The block is being returned - it is allocated and owned
					by the application and has no "next" block. */
					pxBlock->size |= xBlockAllocatedBit;  // Se setea en 1 el bit más significativo
					pxBlock->next = NULL;
					//xNumberOfSuccessfulAllocations++;
				}
			}	 
		}

	return pvReturn;
}


void initHeap()
{
    
    header *pxFirstFreeBlock;
	uint8_t *pucAlignedHeap;
	uint64_t uxAddress;
	uint64_t xTotalHeapSize = TOTAL_HEAP_SIZE;

	/* Ensure the heap starts on a correctly aligned boundary. */
	uxAddress = ( uint64_t ) (uint8_t *) minAddress;

	if( ( uxAddress & BYTE_ALIGNMENT_MASK ) != 0 ) {//si no esta alineado

		uxAddress += ( BYTE_ALIGNMENT - 1 ); //then do it
		uxAddress &= ~( ( uint64_t ) BYTE_ALIGNMENT_MASK );
		xTotalHeapSize -= uxAddress - ( uint64_t ) (uint8_t *) minAddress; //el tamaño es menor
	}

	pucAlignedHeap = ( uint8_t * ) uxAddress;

	/* xStart is used to hold a pointer to the first item in the list of free
	blocks.  The void cast is used to prevent compiler warnings. */
	start.next = ( void * ) pucAlignedHeap;
	start.size = ( uint64_t ) 0;

	/* pxEnd is used to mark the end of the list of free blocks and is inserted
	at the end of the heap space. */
	uxAddress = ( ( uint64_t ) pucAlignedHeap ) + xTotalHeapSize;
	uxAddress -= heapStructSize; //resta el ""header" desde el final
	uxAddress &= ~( ( uint64_t ) BYTE_ALIGNMENT_MASK );
	pEnd = ( void * ) uxAddress;
	pEnd->size= 0;
	pEnd->next = NULL;

	/* To start with there is a single free block that is sized to take up the
	entire heap space, minus the space taken by pxEnd. */
	pxFirstFreeBlock = ( void * ) pucAlignedHeap;
	xTotalSpace = pxFirstFreeBlock->size = uxAddress - ( uint64_t ) pxFirstFreeBlock;
	pxFirstFreeBlock->next = pEnd; //todo el espacio que tengo (sin pxend y sin la partecita alineada) es 1 solo block

	/* Only one block exists - and it covers the entire usable heap space. */
	xMinimumEverFreeBytesRemaining = pxFirstFreeBlock->size;
	xFreeBytesRemaining = pxFirstFreeBlock->size;

	/* Work out the position of the top bit in a uint64_t variable. */
	xBlockAllocatedBit = ( ( uint64_t ) 1 ) << ( ( sizeof( uint64_t ) * heapBITS_PER_BYTE ) - 1 );
}


void RTOSFree(void *ptr)
{

    uint8_t * currp = (uint8_t *) ptr;
    header * insertp;

    if(ptr != NULL)     // The memory being freed will have a header structure immediately before it
    {
        currp -= heapStructSize;
        insertp = (void *) currp;   // Cast avoiding compiler warnings

        if((insertp->size & xBlockAllocatedBit) != 0)
        {
            if(insertp->next == NULL)
            {
                insertp->size &= ~xBlockAllocatedBit;   // The block is no longer allocated
                xFreeBytesRemaining += insertp->size;    // Add this block to the list of free blocks
                addBlockToFreeList(((header *) insertp));
            }
        }
    }
}


static void addBlockToFreeList(header * blockToInsert)
{
    
    header * iteratorp;
    uint8_t * currp;

    iteratorp = &start;
    while(iteratorp->next < blockToInsert)      // Iterate through the list until a block is found that has
    {                                           // a higher address than the block being inserted
        iteratorp = iteratorp->next;
    }

    currp = (uint8_t *) iteratorp;
    if((currp + iteratorp->size) == (uint8_t *) blockToInsert)  // The block being inserted makes a contiguous
    {                                                           // block of memory with the one on its left
        iteratorp->size += blockToInsert->size;                 // So we merge
        blockToInsert = iteratorp;
    }

    currp = (uint8_t *) blockToInsert;
    if((currp + blockToInsert->size) == (uint8_t *) iteratorp->next)    // The block being inserted makes a contiguous
    {                                                                   // block of memory with the one on its right
        if(iteratorp->next != pEnd)                                     // So we merge (if it's not the end)
        {
            blockToInsert->size += iteratorp->next->size;
            blockToInsert->next = iteratorp->next->next;
        } else
        {
            blockToInsert->next = pEnd;
        }
    } else
    {
        blockToInsert->next = iteratorp->next;
    }

    if(iteratorp != blockToInsert)          // If the block was merged with the block before and after him,
    {                                       // then the ptr has been set already and shouldn't change here
        iteratorp->next = blockToInsert;    // (or it would point to itself)
    }
}