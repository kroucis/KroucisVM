//
//  binary.c
//  ObjectVM
//
//  Created by Kyle Roucis on 16-2-28.
//  Copyright © 2016 Kyle Roucis. All rights reserved.
//

#include "binary.h"

#include "class.h"
#include "object.h"
#include "block.h"
#include "vm.h"
#include "integer.h"

#include <assert.h>
#include <string.h>

struct clockwork_binary
{
    struct object_header header;    /* 40 */

    binary_data data;               /* 16 */
    binary_size bytes;              /* 16 */
                                /* = 72 */
};

static void clockwork_binary_alloc_native(object* klass, clockwork_vm* vm)
{
    clkwk_push(vm, (object*)clockwork_binary_init(0x0, 0, vm));
    clkwk_return(vm);
}

static void clockwork_binary_length_native(object* instance, clockwork_vm* vm)
{
    clockwork_binary* binary = (clockwork_binary*)instance;
    integer* len = integer_init(vm, clockwork_binary_length(binary));
    clkwk_push(vm, (object*)len);
    clkwk_return(vm);
}

static void clockwork_binary_dealloc_native(object* instance, clockwork_vm* vm)
{
    clkwk_pushSuper(vm);
    clkwk_dispatch(vm, "dealloc", 0);

    clockwork_binary* binary = (clockwork_binary*)instance;
    clockwork_binary_dealloc(binary, vm);
//    if (clockwork_binary_length(binary) > 0)
//    {
//        clkwk_free(vm, (void*)binary->data);
//    }
//    clkwk_free(vm, instance);
    clkwk_pop(vm);
    clkwk_pushNil(vm);
    clkwk_return(vm);
}

struct class* clockwork_binary_class(struct clockwork_vm* vm)
{
    class* clockworkBinaryClass = class_init(vm, "ClockworkBinary", "Object");

    // Class Methods
    {
        class_addClassMethod(clockworkBinaryClass, vm, "alloc", block_init_native(vm, 0, 0, &clockwork_binary_alloc_native));
    }

    // Instance Methods
    {
        class_addInstanceMethod(clockworkBinaryClass, vm, "length", block_init_native(vm, 0, 0, &clockwork_binary_length_native));
        class_addInstanceMethod(clockworkBinaryClass, vm, "dealloc", block_init_native(vm, 0, 0, &clockwork_binary_dealloc_native));
    }

    return clockworkBinaryClass;
}

clockwork_binary* clockwork_binary_init(const char* const data, uint64_t length, struct clockwork_vm* vm)
{
    assert(vm);
    assert(data);

    object* binSuper = object_init(vm);
    clockwork_binary* binary = (clockwork_binary*)object_create_super(vm, binSuper, (class*)clkwk_getConstant(vm, "ClockworkBinary"), sizeof(clockwork_binary));
    if (length > 0)
    {
        binary->data = clkwk_allocate(vm, length);
        memcpy((void*)binary->data, (void*)data, length);
    }
    binary->bytes = length;
    binary->header.size = sizeof(clockwork_binary);

    return binary;
}

void clockwork_binary_dealloc(clockwork_binary* bin, struct clockwork_vm* vm)
{
    assert(bin);
    assert(vm);

    if (clockwork_binary_length(bin) > 0)
    {
        clkwk_free(vm, (void*)bin->data);
    }
    clkwk_free(vm, (object*)bin);
}

uint64_t clockwork_binary_length(clockwork_binary* bin)
{
    return bin->bytes;
}

const char* const clockwork_binary_data(clockwork_binary* bin)
{
    return bin->data;
}
