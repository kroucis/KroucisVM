//
//  instruction.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-16.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include <inttypes.h>

struct str;

typedef struct
{
    uint8_t op;
    char* params[5];
    uint8_t param_count;
} instruction;

typedef struct
{
    uint32_t inst_count;
    instruction* instructions;
} instruction_sequence;
