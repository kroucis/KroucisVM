//
//  assembler.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-16.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include "instruction.h"
#include "block.h"

struct clockwork_vm;

enum
{
    clkwk_NOOP = 0,
    clkwk_GOTO,
    clkwk_GOTO_IF_FALSE,
    clkwk_GOTO_IF_TRUE,
    clkwk_POP,
    clkwk_PUSH_NIL,
    clkwk_PUSH_TRUE,
    clkwk_PUSH_FALSE,
    clkwk_PUSH_LOCAL,
    clkwk_SET_LOCAL,
    clkwk_POP_TO_LOCAL,
    clkwk_PUSH_SELF,
    clkwk_PUSH_SUPER,
    clkwk_PUSH_IVAR,
    clkwk_SET_IVAR,
    clkwk_PRINT,
    clkwk_PUSH_STRING,
    clkwk_PUSH_INT,
    clkwk_PUSH_NUMBER,
    clkwk_PUSH_CONSTANT,
    clkwk_RETURN,
};

void assembler_run_instruction(instruction*, struct clockwork_vm*);
void assembler_run_block(block*, struct clockwork_vm*);
