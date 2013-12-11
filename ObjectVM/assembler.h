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
    VM_NOOP = 0,
    VM_GOTO,
    VM_GOTO_IF_FALSE,
    VM_GOTO_IF_TRUE,
    VM_POP,
    VM_PUSH_NIL,
    VM_PUSH_TRUE,
    VM_PUSH_FALSE,
    VM_PUSH_LOCAL,
    VM_SET_LOCAL,
    VM_POP_TO_LOCAL,
    VM_PUSH_SELF,
    VM_PUSH_SUPER,
    VM_PUSH_IVAR,
    VM_SET_IVAR,
    VM_PRINT,
    VM_PUSH_STRING,
    VM_PUSH_INT,
    VM_PUSH_NUMBER,
    VM_PUSH_CONSTANT,
    VM_RETURN,
};

void assembler_run_instruction(instruction*, struct clockwork_vm*);
void assembler_run_block(block*, struct clockwork_vm*);
