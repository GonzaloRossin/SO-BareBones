#ifdef MEM_MAN_H
#include "mem_man.h"

/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * A sample implementation of malloc() and free() that combines
 * (coalescences) adjacent memory blocks as they are freed, and in so doing
 * limits memory fragmentation.
 *
 * See heap_1.c, heap_2.c and heap_3.c for alternative implementations, and the
 * memory management pages of http://www.FreeRTOS.org for more information.
 */


/*
 * Inserts a block of memory that is being freed into the correct position in
 * the list of free memory blocks.  The block being freed will be merged with
 * the block in front it and/or the block behind it if the memory blocks are
 * adjacent to each other.
 */
static void prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert );

/*
 * Called automatically to setup the required heap structures the first time
 * malloc() is called.
 */
static void prvHeapInit( void );

/*-----------------------------------------------------------*/

/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static const uint64_t xHeapStructSize	= ( sizeof( BlockLink_t ) + ( ( uint64_t ) ( portBYTE_ALIGNMENT - 1 ) ) ) & ~( ( uint64_t ) portBYTE_ALIGNMENT_MASK );

/* Create a couple of list links to mark the start and end of the list. */
static BlockLink_t xStart, *pxEnd = NULL;

static void const * maxAddress = (void *) MAX_ADDRESS;

static void const * minAddress = (void *) MIN_ADDRESS;

static char const * sys_name = "FirstFit";

/* Keeps track of the number of calls to allocate and free memory as well as the
number of free bytes remaining, but says nothing about fragmentation. */

static uint64_t xTotalSpace = 0;
static uint64_t xFreeBytesRemaining = 0;
static uint64_t xMinimumEverFreeBytesRemaining = 0U;
static uint64_t xNumberOfSuccessfulAllocations = 0;
static uint64_t xNumberOfSuccessfulFrees = 0;

/* Gets set to the top bit of an uint64_t type.  When this bit in the xBlockSize
member of an BlockLink_t structure is set then the block belongs to the
application.  When the bit is free the block is still part of the free heap
space. */
static uint64_t xBlockAllocatedBit = 0;

/*-----------------------------------------------------------*/

void* RTOSMalloc( uint64_t xWantedSize ) {
BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
void *pvReturn = NULL;

		/* If this is the first call to malloc then the heap will require
		initialization to setup the list of free blocks. */
		if( pxEnd == NULL )
			prvHeapInit();

		/* Check the requested block size is not so large that the top bit is
		set.  The top bit of the block size member of the BlockLink_t structure
		is used to determine who owns the block - the application or the
		kernel, so it must be free. */
		if( ( xWantedSize & xBlockAllocatedBit ) == 0 ) {//entra cuando wanted size es < 2 a la n-1
 
			/* The wanted size is increased so it can contain a BlockLink_t
			structure in addition to the requested amount of bytes. */
			if( xWantedSize > 0 ) {

				xWantedSize += xHeapStructSize;

				/* Ensure that blocks are always aligned to the required number
				of bytes. */
				if( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) != 0x00 ) {
					/* Byte alignment required. */
					xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
				}
			}
		 

			if( ( xWantedSize > 0 ) && ( xWantedSize <= xFreeBytesRemaining ) ) {
				/* Traverse the list from the start	(lowest address) block until
				one	of adequate size is found. */
				pxPreviousBlock = &xStart;
				pxBlock = xStart.pxNextFreeBlock;
				while( ( pxBlock->xBlockSize < xWantedSize ) && ( pxBlock->pxNextFreeBlock != NULL ) ) {//CORTA SI ENCUENTRA O BLOCK O SI LLEGA LA FINAL
					pxPreviousBlock = pxBlock;
					pxBlock = pxBlock->pxNextFreeBlock;
				}

				/* If the end marker was reached then a block of adequate size
				was	not found. */
				if( pxBlock != pxEnd ) {//ENCONTRASTE UN BLOQUE

					/* Return the memory space pointed to - jumping over the
					BlockLink_t structure at its start. */
					pvReturn = ( void * ) ( ( ( uint8_t * ) pxPreviousBlock->pxNextFreeBlock ) + xHeapStructSize );

					/* This block is being returned for use so must be taken out
					of the list of free blocks. */
					pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

					/* If the block is larger than required it can be split into
					two. */
					if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE ) {

						/* This block is to be split into two.  Create a new
						block following the number of bytes requested. The void
						cast is used to prevent byte alignment warnings from the
						compiler. */
						pxNewBlockLink = ( void * ) ( ( ( uint8_t * ) pxBlock ) + xWantedSize );
						
						/* Calculate the sizes of two blocks split from the
						single block. */
						pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
						pxBlock->xBlockSize = xWantedSize;

						/* Insert the new block into the list of free blocks. */
						prvInsertBlockIntoFreeList( pxNewBlockLink );
					}

					xFreeBytesRemaining -= pxBlock->xBlockSize;

					if( xFreeBytesRemaining < xMinimumEverFreeBytesRemaining ) {
						xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
					}

					/* The block is being returned - it is allocated and owned
					by the application and has no "next" block. */
					pxBlock->xBlockSize |= xBlockAllocatedBit;  // Se setea en 1 el bit más significativo
					pxBlock->pxNextFreeBlock = NULL;
					xNumberOfSuccessfulAllocations++;
				}
			}	 
		}

	return pvReturn;
}
/*-----------------------------------------------------------*/

void RTOSFree( void *pv ) {
	uint8_t *puc = ( uint8_t * ) pv;
	BlockLink_t *pxLink;

	if( pv != NULL ) {
		
		/* The memory being freed will have an BlockLink_t structure immediately
		before it. */
		puc -= xHeapStructSize;

		/* This casting is to keep the compiler from issuing warnings. */
		pxLink = ( void * ) puc;

		/* Check the block is actually allocated. */

		if( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 ) {

			if( pxLink->pxNextFreeBlock == NULL ) {

				/* The block is being returned to the heap - it is no longer
				allocated. */
				pxLink->xBlockSize &= ~xBlockAllocatedBit; //TENIAS UN 1000N, negas a xblockallocated(100000), queda en 011111, con lo cual al hacer & nos queda 0000N 
				/* Add this block to the list of free blocks. */
				xFreeBytesRemaining += pxLink->xBlockSize;

				prvInsertBlockIntoFreeList( ( ( BlockLink_t * ) pxLink ) );
				xNumberOfSuccessfulFrees++;
				xNumberOfSuccessfulAllocations--;
			}
		}
	}
}
/*-----------------------------------------------------------*/

static void prvHeapInit( void ) {
	BlockLink_t *pxFirstFreeBlock;
	uint8_t *pucAlignedHeap;
	uint64_t uxAddress;
	uint64_t xTotalHeapSize = configTOTAL_HEAP_SIZE;

	/* Ensure the heap starts on a correctly aligned boundary. */
	uxAddress = ( uint64_t ) (uint8_t *) minAddress;

	if( ( uxAddress & portBYTE_ALIGNMENT_MASK ) != 0 ) {//si no esta alineado

		uxAddress += ( portBYTE_ALIGNMENT - 1 ); //then do it
		uxAddress &= ~( ( uint64_t ) portBYTE_ALIGNMENT_MASK );
		xTotalHeapSize -= uxAddress - ( uint64_t ) (uint8_t *) minAddress; //el tamaño es menor
	}

	pucAlignedHeap = ( uint8_t * ) uxAddress;

	/* xStart is used to hold a pointer to the first item in the list of free
	blocks.  The void cast is used to prevent compiler warnings. */
	xStart.pxNextFreeBlock = ( void * ) pucAlignedHeap;
	xStart.xBlockSize = ( uint64_t ) 0;

	/* pxEnd is used to mark the end of the list of free blocks and is inserted
	at the end of the heap space. */
	uxAddress = ( ( uint64_t ) pucAlignedHeap ) + xTotalHeapSize;
	uxAddress -= xHeapStructSize; //resta el ""header" desde el final
	uxAddress &= ~( ( uint64_t ) portBYTE_ALIGNMENT_MASK );
	pxEnd = ( void * ) uxAddress;
	pxEnd->xBlockSize = 0;
	pxEnd->pxNextFreeBlock = NULL;

	/* To start with there is a single free block that is sized to take up the
	entire heap space, minus the space taken by pxEnd. */
	pxFirstFreeBlock = ( void * ) pucAlignedHeap;
	pxFirstFreeBlock->xBlockSize = uxAddress - ( uint64_t ) pxFirstFreeBlock;
	xTotalSpace=pxFirstFreeBlock->xBlockSize;
	pxFirstFreeBlock->pxNextFreeBlock = pxEnd; //todo el espacio que tengo (sin pxend y sin la partecita alineada) es 1 solo block

	/* Only one block exists - and it covers the entire usable heap space. */
	xMinimumEverFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;
	xFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;

	/* Work out the position of the top bit in a uint64_t variable. */
	xBlockAllocatedBit = ( ( uint64_t ) 1 ) << ( ( sizeof( uint64_t ) * heapBITS_PER_BYTE ) - 1 );
}
/*-----------------------------------------------------------*/

static void prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert ) {

	BlockLink_t *pxIterator;
	uint8_t *puc;

	/* Iterate through the list until a block is found that has a higher address
	than the block being inserted. */
	for( pxIterator = &xStart; pxIterator->pxNextFreeBlock < pxBlockToInsert; pxIterator = pxIterator->pxNextFreeBlock );
		/* Nothing to do here, just iterate to the right position. */

	/* Do the block being inserted, and the block it is being inserted after
	make a contiguous block of memory? */ //AVOID FRAGMENTACION EXTERNA 
	puc = ( uint8_t * ) pxIterator;
	if( ( puc + pxIterator->xBlockSize ) == ( uint8_t * ) pxBlockToInsert ) {
		pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
		pxBlockToInsert = pxIterator;
	}
	 

	/* Do the block being inserted, and the block it is being inserted before
	make a contiguous block of memory? */
	puc = ( uint8_t * ) pxBlockToInsert;
	if( ( puc + pxBlockToInsert->xBlockSize ) == ( uint8_t * ) pxIterator->pxNextFreeBlock ) {

		if( pxIterator->pxNextFreeBlock != pxEnd ) {

			/* Form one big block from the two blocks. */
			pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
			pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
		}
		else {
			pxBlockToInsert->pxNextFreeBlock = pxEnd;
		}
	}
	else {
		pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
	}

	/* If the block being inserted plugged a gab, so was merged with the block
	before and the block after, then it's pxNextFreeBlock pointer will have
	already been set, and should not be set here as that would make it point
	to itself. */
	if( pxIterator != pxBlockToInsert )	{
		pxIterator->pxNextFreeBlock = pxBlockToInsert;
	}
 
}  
 

void getMMStats(mm_stat* stats) {
	if( pxEnd == NULL )
		prvHeapInit();
	stats->free=xFreeBytesRemaining;
	stats->total=xTotalSpace;
	stats->occupied=xTotalSpace-xFreeBytesRemaining;
	stats->successful_allocs=xNumberOfSuccessfulAllocations;
	stats->successful_frees=xNumberOfSuccessfulFrees;
	stats->sys_name=(char*) sys_name;
}
#endif
