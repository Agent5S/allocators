//
//  TempAlloc.h
//  Allocators
//
//  Created by Agent5S on 26/1/23.
//

#ifndef TempAlloc_h
#define TempAlloc_h

#include <stdlib.h>

///Max number of allowed scopes
static const uint8_t TEMP_ALLOC_MAX_SCOPES = 8;

/**
 Represents a temporary allocator
 */
typedef struct {
    ///Pointer to the current free byte in the allocator
    void* current;
    ///Pointer to the last byte in the allocator
    void* end;
    ///Number of current nested allocation scopes
    uint8_t scopeIdx;
    ///Stack of allocation scope pointers
    void* scopes[TEMP_ALLOC_MAX_SCOPES];
} TempAlloc;

/**
 Initialize a temporary allocator
 */
uint8_t TempAlloc_Init(TempAlloc* , size_t);

/**
 Deinitialize a temporary allocator
 */
void TempAlloc_Deinit(TempAlloc*);

/**
 Begin a new allocator scope
 
 Each level on nested function calls shoud begin a new scope
 and should end the scope before returning.
 */
void TempAlloc_Begin(TempAlloc*);

/**
 End an allocator scope
 
 Functions that begin a new scope should end it before returning
 */
void TempAlloc_End(TempAlloc*);

/**
 Allocate a given number of bytes
 */
void* TempAlloc_Get(TempAlloc*, size_t);

#endif /* TempAlloc_h */
