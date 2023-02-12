//
//  ChunkAlloc.c
//  Allocators
//
//  Created by Agent5S on 27/1/23.
//

#include <ChunkAlloc.h>
#include <stdlib.h>
#include <assert.h>

uint8_t ChunkAlloc_Init(ChunkAlloc* alloc, size_t itemSize, size_t maxItems) {
    uint8_t sizeAsBigAsPtr = itemSize >= sizeof(void*);
    assert(sizeAsBigAsPtr);
    if (!sizeAsBigAsPtr) {
        return 1;
    }

    alloc->pool = malloc(itemSize * maxItems);
    if (!alloc->pool) {
        return 1;
    }
    
    for (int i = 0; i < maxItems - 1; i++) {
        void* chunk = (void*)alloc->pool + (i * itemSize);
        *(void**)chunk = alloc->pool + ((i + 1) * itemSize);
    }
    void* chunk = (void*)alloc->pool + ((maxItems - 1) * itemSize);
    *(void**)chunk = NULL;
    
    alloc->current = alloc->pool;
    alloc->itemSize = itemSize;
    alloc->maxItems = maxItems;
    
    return 0;
}

void ChunkAlloc_Deinit(ChunkAlloc* alloc) {
    free(alloc->pool);
    
    alloc->pool = NULL;
    alloc->current = NULL;
    alloc->itemSize = 0;
    alloc->maxItems = 0;
}

void* ChunkAlloc_Get(ChunkAlloc* alloc) {
    assert(alloc->current);
    if (!alloc->current) {
        return NULL;
    }
    
    void* ret = alloc->current;
    void* current = *((void**)alloc->current);
    alloc->current = current;
        
    return ret;
}

void ChunkAlloc_Free(ChunkAlloc* alloc, void* item) {
    uint8_t insidePool = item >= alloc->pool && item < (alloc->pool + (alloc->itemSize * alloc->maxItems));
    uint8_t notCurrent = item != alloc->current;
    assert(insidePool);
    assert(notCurrent);
    if (!insidePool && !notCurrent) {
        return;
    }
    
    *(void**)item = alloc->current;
    alloc->current = item;
}
