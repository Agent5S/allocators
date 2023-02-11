//
//  ChunkAlloc.h
//  Allocators
//
//  Created by Agent5S on 27/1/23.
//

#ifndef ChunkAlloc_h
#define ChunkAlloc_h

#include <stdlib.h>

/**
 Represents a chunk allocator
 */
typedef struct {
    ///Pointer to the current free item in the pool
    void* current;
    ///Size of each item in the pool
    size_t itemSize;
    ///Max number of items that fit in the pool
    size_t maxItems;
    ///Pointer to the memory pool
    void* pool;
} ChunkAlloc;

/**
 Initialize a chunk allocator
 */
uint8_t ChunkAlloc_Init(ChunkAlloc*, size_t, size_t);

/**
 Deinitialize a temporary allocator
*/
void ChunkAlloc_Deinit(ChunkAlloc*);

/**
 Allocate an item in the pool
 */
void* ChunkAlloc_Get(ChunkAlloc*);

/**
 Free an item from the pool
 */
void ChunkAlloc_Free(ChunkAlloc*, void*);

#endif /* ChunkAlloc_h */
