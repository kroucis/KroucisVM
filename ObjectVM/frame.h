//
//  frame.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-12-8.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include <stdint.h>

static const uint8_t c_LocalsLimit = 8;
static const uint8_t c_UpValsLimit = c_LocalsLimit;

struct object;

typedef struct frame
{
    object* frameSelf;
    uint64_t returnPC;
    object* locals[c_LocalsLimit];
    object* upvals[c_UpValsLimit];
} frame;

typedef struct frame_stack
{
    frame frames[32];
    uint8_t idx;
} frame_stack;
