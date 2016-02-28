//
//  binary.h
//  ObjectVM
//
//  Created by Kyle Roucis on 16-2-28.
//  Copyright © 2016 Kyle Roucis. All rights reserved.
//

#pragma once

#include <inttypes.h>

#include "binary.h"

struct clockwork_vm;

typedef struct clockwork_binary clockwork_binary;
typedef const char* binary_data;
typedef uint64_t binary_size;

clockwork_binary* clockwork_binary_init(const char* const data, uint64_t length, struct clockwork_vm* vm);
void clockwork_binary_dealloc(clockwork_binary* bin, struct clockwork_vm* vm);
uint64_t clockwork_binary_length(clockwork_binary* bin);
const char* const clockwork_binary_data(clockwork_binary* bin);

