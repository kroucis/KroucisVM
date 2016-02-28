//
//  binary.c
//  ObjectVM
//
//  Created by Kyle Roucis on 16-2-28.
//  Copyright © 2016 Kyle Roucis. All rights reserved.
//

#include "binary.h"

#include "vm.h"

struct clockwork_binary
{
    binary_data data;
    binary_size bytes;
};

clockwork_binary* clockwork_binary_init(const char* const data, uint64_t length, struct clockwork_vm* vm)
{
    clockwork_binary* bin = clkwk_allocate(vm, sizeof(clockwork_binary));
    bin->data = (char*)data;
    bin->bytes = length;

    return bin;
}

void clockwork_binary_dealloc(clockwork_binary* bin, struct clockwork_vm* vm)
{
    clkwk_free(vm, bin);
}

uint64_t clockwork_binary_length(clockwork_binary* bin)
{
    return bin->bytes;
}

const char* const clockwork_binary_data(clockwork_binary* bin)
{
    return bin->data;
}

