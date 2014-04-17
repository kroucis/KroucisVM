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
static const uint8_t c_StackLimit = 32;

struct object;

#warning Consider making call frames dynamically allocated? This is a LOT of up-front memory...

typedef struct frame
{
    object* frameSelf;                  /* 8 */
    uint64_t returnPC;                  /* 8 */
    object* locals[c_LocalsLimit];      /* 8 x 8 = 64 */
    object* upvals[c_UpValsLimit];      /* 8 x 8 = 64 */
                                    /* = 144 */
} frame;

typedef struct frame_stack
{
    frame frames[c_StackLimit];                   /* 144 x 32 = 4608 */
    uint8_t idx;                        /* 1 */
//    uint8_t padding[7];               /* 7 */
                                /* = 4616 */
} frame_stack;
