//
//  TempAlloc.c
//  Allocators
//
//  Created by Agent5S on 26/1/23.
//

#include <TempAlloc.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

uint8_t TempAlloc_Init(TempAlloc* alloc, size_t size) {
    alloc->current = malloc(size);
    if (!alloc->current) {
        return 1;
    }
    alloc->end = alloc->current + size - 1;
    alloc->scopeIdx = 0;
    memset(&alloc->scopes, 0, sizeof(void *) * TEMP_ALLOC_MAX_SCOPES);
    
    return 0;
}

void TempAlloc_Deinit(TempAlloc* alloc) {
    free(alloc->current);
    
    alloc->current = NULL;
    alloc->end = NULL;
    alloc->scopeIdx = 0;
    memset(&alloc->scopes, 0, sizeof(void *) * TEMP_ALLOC_MAX_SCOPES);
}

void TempAlloc_Begin(TempAlloc* alloc) {
    assert(alloc->scopeIdx < TEMP_ALLOC_MAX_SCOPES);
    
    alloc->scopes[alloc->scopeIdx] = alloc->current;
    alloc->scopeIdx++;
}

void TempAlloc_End(TempAlloc* alloc) {
    assert(alloc->scopeIdx > 0);
    
    alloc->scopeIdx--;
    alloc->current = alloc->scopes[alloc->scopeIdx];
    alloc->scopes[alloc->scopeIdx] = NULL;
}

void* TempAlloc_Get(TempAlloc* alloc, size_t size) {
    void* next = alloc->current + size;
    
    assert(next <= alloc->end);
    
    void* previous = alloc->current;
    alloc->current = next;
    
    return previous;
}
