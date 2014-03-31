//
//  frame.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-12-8.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include <stdint.h>

struct object;

typedef struct frame
{
    object* frameSelf;
//    uint64_t returnPC;
} frame;

typedef struct frame_stack
{
    frame frames[32];
    uint8_t idx;
} frame_stack;
