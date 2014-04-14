//
//  block.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-17.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "block.h"

#include "instruction.h"
#include "class.h"
#include "object.h"
#include "str.h"
#include "vm.h"
#include "primitive_table.h"

#include "clkwk_debug.h"

#include <stdlib.h>
#include <memory.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>

//struct local_scope
//{
//    uint8_t count;
//    char** locals;
//};

#pragma mark Block

enum
{
    BlockTypeNative = 0,
    BlockTypeByteCodeCompiled,
};

struct block
{
    struct object_header header;            /* 40 */
    union
    {
        native_block nativeFunc;
        uint64_t absPC;
    } impl;                                 /* 8 */
    uint8_t localsCount;                    /* 1 */
    uint8_t upvalsCount;                    /* 1 */

    uint8_t type;

    uint8_t _padding[7];                    /* 7 */
                                        /* = 57 (7) */
};

#pragma mark - Native Methods

class* block_class(clockwork_vm* vm)
{
    class* block_class = class_init(vm, "Block", "Object");

#ifdef CLKWK_PRINT_SIZES
    CLKWK_DBGPRNT("Block: %lu\n", sizeof(block));
#endif

    return block_class;
}

block* block_init_native(struct clockwork_vm* vm, uint8_t localsCount, uint8_t upvalsCount, native_block func)
{
    assert(vm);
    assert(func);

    block* m = clkwk_allocate(vm, sizeof(block));
    m->localsCount = localsCount;
    m->upvalsCount = upvalsCount;
    m->type = BlockTypeNative;
    m->impl.nativeFunc = func;
    m->header.size = sizeof(block);

    return m;
}

block* block_init_compiled(struct clockwork_vm* vm, uint8_t localsCount, uint8_t upvalsCount, uint64_t pc)
{
    assert(vm);

    block* m = clkwk_allocate(vm, sizeof(block));
    m->localsCount = localsCount;
    m->upvalsCount = upvalsCount;
    m->type = BlockTypeByteCodeCompiled;
    m->impl.absPC = pc;
    m->header.size = sizeof(block);

    return m;
}

void block_dealloc(block* instance, clockwork_vm* vm)
{
    assert(instance);
    assert(vm);

    clkwk_free(vm, (object*)instance);
}

native_block block_nativeFunction(block* blk, clockwork_vm* vm)
{
    assert(blk);
    assert(vm);

    if (blk->type == BlockTypeNative)
    {
        return blk->impl.nativeFunc;
    }
    else
    {
        return NULL;
    }
}

uint8_t block_localsCount(block* blk, clockwork_vm* vm)
{
    assert(blk);
    assert(vm);

    return blk->localsCount;
}

uint8_t block_upvalsCount(block* blk, clockwork_vm* vm)
{
    assert(blk);
    assert(vm);

    return blk->upvalsCount;
}

uint64_t block_pcLocation(block* blk, clockwork_vm* vm)
{
    assert(blk);
    assert(vm);

    if (blk->type == BlockTypeByteCodeCompiled)
    {
        return blk->impl.absPC;
    }
    else
    {
        return UINT64_MAX;
    }
}
