//
//  memory_manager.c
//  ObjectVM
//
//  Created by Kyle Roucis on 14-4-2.
//  Copyright (c) 2014 Kyle Roucis. All rights reserved.
//

#include "memory_manager.h"

#ifdef CLKWK_DEBUG_MEMORY
    #include <stdio.h>
#endif

#include <memory.h>

struct mem_block
{
    size_t size;
    void* memory;
};

#ifdef CLKWK_TRACK_MEMORY
static size_t s_count = 0;
#endif

void* _debug_malloc(const char* file, int line, size_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    int rem = size % sizeof(void*) != 0;
    if (rem != 0)
    {
        size -= rem;
        size += sizeof(void*);
    }

#ifndef CLKWK_TRACK_MEMORY
    void* ptr = calloc(1, size);
    return ptr;

#else
    size = size + sizeof(size_t);
    void* ptr = calloc(1, size);
    if (ptr)
    {
        s_count += size;

#ifdef CLKWK_DEBUG_MEMORY
        printf("Allocating %zu bytes. Total: %zu\n", size, s_count);
#endif

        struct mem_block* mem = (struct mem_block*)ptr;

        mem->size = size;
        mem->memory = ptr + sizeof(size_t);

        return ptr + sizeof(size_t);
    }
    else
    {
        return NULL;
    }
#endif
}

void _debug_free(const char* file, int line, void* ptr)
{
    if (!ptr)
    {
        return;
    }

#ifndef CLKWK_TRACK_MEMORY

    free(ptr);

#else

    struct mem_block* mem = (struct mem_block*)(ptr - sizeof(size_t));

    s_count -= mem->size;

#ifdef DEBUG_MEMORY
    printf("Freeing %zu bytes. Total: %zu\n", mem->size, s_count);
#endif

    free(mem);
#endif
}

//#include <stdlib.h>
//
//enum
//{
//    MEM_FREE = 0,
//    MEM_ALLOCATED = 1,
//};
//
//struct memory_block
//{
//    uint8_t flags;
//    uint16_t size;
//    memory_block* next;
//    void* data;
//};
//
//struct memory_manager
//{
//    uint32_t poolSize;
//    uint8_t* memory;
//    uint8_t** freeArray;
//    uint8_t** allocdArray;
//};
//
//static const uint8_t c_block_size = 32;     // bytes
//
//memory_manager* memory_manager_init(uint32_t maxSize)
//{
//    memory_manager* mman = malloc(sizeof(memory_manager));
//    mman->poolSize = maxSize;
//    mman->memory = calloc(maxSize, sizeof(uint8_t));
//    uint16_t count = maxSize / 32;
//
//    return mman;
//}
//
//memory_block* memory_manager_alloc(memory_manager* mman, uint16_t size)
//{
//    return NULL;
//}
//
//void memory_manager_free(memory_manager* mman, memory_block* block)
//{
//
//}
//
//uint16_t memory_block_size(memory_block* block)
//{
//    return block->size;
//}
//
//void* memory_block_data(memory_block* block)
//{
//    return block->data;
//}
