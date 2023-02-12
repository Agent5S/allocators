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

 @param alloc Pointer to allocator
 @param itemSize Size in bytes each item in the pool should have, 
 should be at least as big as the platform's pointer size
 @param maxItems Maximum number of items the pool should hold

 @return 0 on success 1 on error
 */
uint8_t ChunkAlloc_Init(ChunkAlloc* alloc, size_t itemSize, size_t maxItems);

/**
 Deinitialize a temporary allocator

 @param alloc Pointer to allocator
 */
void ChunkAlloc_Deinit(ChunkAlloc* alloc);

/**
 Allocate an item in the pool

 @param alloc Pointer to the allocator

 @return Pointer to the allocated chunk o NULL if the pool is full
 */
void* ChunkAlloc_Get(ChunkAlloc* alloc);

/**
 Free an item from the pool

 @param alloc Pointer to the allocator
 @param item Pointer to the chunk to free

 @todo Fail if an unaligned pointer is passed
 */
void ChunkAlloc_Free(ChunkAlloc* alloc, void* item);

#endif /* ChunkAlloc_h */
