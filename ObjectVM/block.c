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
#include "integer.h"
#include "array.h"

#include "clkwk_debug.h"

#include <stdlib.h>
#include <memory.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>

#pragma mark Block

static const char* c_BlockClassName = "Block";

struct enclosing_scope
{
//    class* declaringClass;                  /* 8 */       ?
    object** upvals;                        /* 8 */
    object* enclosingSelf;                  /* 8 */
    object* enclosingSuper;                 /* 8 */
    uint8_t numUpvals;                      /* 1 */
    uint8_t _padding[7];                    /* 7 */
                                        /* = 40 */
};

enum
{
    BlockTypeNative = 0,
    BlockTypeByteCodeCompiled,
};

struct block
{
    struct object_header header;            /* 40 */
    struct enclosing_scope enclosingScope;  /* 40 */
    union
    {
        native_block nativeFunc;
        uint64_t absPC;
    } impl;                                 /* 8 */

    uint8_t localsCount;                    /* 1 */
    uint8_t type;                           /* 1 */

    uint8_t _padding[6];                    /* 6 */
                                        /* = 96 (6) */
};

#pragma mark - Bound Methods

static void block_alloc_native(object* blockClass, clockwork_vm* vm)
{
    object* sup = object_init(vm);
    block* b = (block*)object_create_super(vm, sup, (class*)clkwk_getConstant(vm, c_BlockClassName), sizeof(block));

    clkwk_push(vm, (object*)b);
    clkwk_return(vm);
}

static void block_initWithNumArgs_upVals_atPC_enclosedSelf_enclosedSuper_native(object* slf, clockwork_vm* vm)
{
    integer* args = (integer*)clkwk_getLocal(vm, 0);
    array* upvals = (array*)clkwk_getLocal(vm, 1);
    integer* pc = (integer*)clkwk_getLocal(vm, 2);
    object* encSup = clkwk_getLocal(vm, 3);
    object* encSlf = clkwk_getLocal(vm, 4);
//    class* dclClass = clkwk_getLocal(vm, 0);          ?
    uint8_t localsCount = (uint8_t)integer_toInt64(args, vm);
    block* b = (block*)slf;

    b->type = BlockTypeByteCodeCompiled;

    b->enclosingScope.enclosingSelf = encSlf;
    b->enclosingScope.enclosingSuper = encSup;

    if (!object_isNil((object*)upvals, vm))
    {
        uint64_t upvalCount = array_count(upvals, vm);
        b->enclosingScope.numUpvals = (uint8_t)upvalCount;
        b->enclosingScope.upvals = clkwk_allocate(vm, sizeof(object*) * upvalCount);
        for (uint64_t i = 0; i < upvalCount; i++)
        {
            b->enclosingScope.upvals[i] = array_objectAtIndex(upvals, vm, i);
        }
    }

    b->impl.absPC = integer_toInt64(pc, vm);
    b->localsCount = localsCount;

    clkwk_push(vm, (object*)b);
    clkwk_return(vm);
}

static void block_invoke_native(object* slf, clockwork_vm* vm)
{
    block* b = (block*)slf;
    clkwk_jump(vm, b->impl.absPC);
}

#pragma mark - Native Methods

class* block_class(clockwork_vm* vm)
{
    class* block_class = class_init(vm, c_BlockClassName, "Object");

    // Class Methods
    {
        class_addClassMethod(block_class, vm, "alloc", block_init_native(vm, 0, 0, &block_alloc_native));
    }

    // Instance Methods
    {
        class_addInstanceMethod(block_class, vm, "initWithNumArgs:upVals:atPC:enclosedSelf:enclosedSuper:", block_init_native(vm, 5, 0, &block_initWithNumArgs_upVals_atPC_enclosedSelf_enclosedSuper_native));
        class_addInstanceMethod(block_class, vm, "invoke", block_init_native(vm, 0, 0, &block_invoke_native));
    }

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
    m->type = BlockTypeNative;
    m->impl.nativeFunc = func;
    m->header.size = sizeof(block);

    return m;
}

block* block_init_compiled(struct clockwork_vm* vm, uint8_t localsCount, uint8_t upvalsCount, struct object** upvals, uint64_t pc, struct object* enclosedSelf, struct object* enclosedSuper) // , struct class* declaringClass) ?
{
    block* b = clkwk_allocate(vm, sizeof(block));
    b->enclosingScope.numUpvals = upvalsCount;
    b->enclosingScope.enclosingSelf = enclosedSelf;
    b->enclosingScope.enclosingSuper = enclosedSuper;
//    b->enclosingScope.declaringClass = declaringClass;            ?
    b->enclosingScope.upvals = upvals;

    b->localsCount = localsCount;
    b->type = BlockTypeByteCodeCompiled;
    b->impl.absPC = pc;
    b->header.size = sizeof(block);

    return b;
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

    return blk->enclosingScope.numUpvals;
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
