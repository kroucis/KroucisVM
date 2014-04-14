//
//  memory_manager.h
//  ObjectVM
//
//  Created by Kyle Roucis on 14-4-2.
//  Copyright (c) 2014 Kyle Roucis. All rights reserved.
//

#pragma once

#define CLKWK_DEBUG_MEMORY
//#define CLKWK_TRACK_MEMORY

#define MEM_MALLOC(size)    _debug_malloc(__FILE__, __LINE__, (size))
#define MEM_FREE(ptr)       _debug_free(__FILE__, __LINE__, (ptr))

#include <stdlib.h>

void* _debug_malloc(const char* file, int line, size_t size);
void _debug_free(const char* file, int line, void* ptr);

//#include <stdint.h>
//
//typedef struct memory_manager memory_manager;
//typedef struct memory_block memory_block;
//
//memory_manager* memory_manager_init(uint32_t maxSize);
//memory_block* memory_manager_alloc(memory_manager* mman, uint16_t size);
//void memory_manager_free(memory_manager* mman, memory_block* block);
//
//uint16_t memory_block_size(memory_block* block);
//void* memory_block_data(memory_block* block);
