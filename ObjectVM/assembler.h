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

struct str;
struct clockwork_vm;

enum
{
    clkwk_NOOP = 0,         // noop
    clkwk_JUMP,             // jump @
    clkwk_JUMP_IF_FALSE,    // jmpf @
    clkwk_JUMP_IF_TRUE,     // jmpt @
    clkwk_POP,              // pop
    clkwk_PUSH_NIL,         // nil
    clkwk_PUSH_TRUE,        // true
    clkwk_PUSH_FALSE,       // false
    clkwk_PUSH_LOCAL,       // pushl :
    clkwk_PUSH_CLOCKWORK,   // clkwk
    clkwk_SET_LOCAL,        // setl :
    clkwk_POP_TO_LOCAL,     // popl :
    clkwk_PUSH_SELF,        // self
    clkwk_PUSH_SUPER,       // super
    clkwk_PUSH_IVAR,        // pushi :
    clkwk_SET_IVAR,         // seti :
    clkwk_PRINT,            // ?
    clkwk_PUSH_STRING,      // push "
    clkwk_PUSH_INT,         // push #
    clkwk_PUSH_NUMBER,      // push #
    clkwk_PUSH_SYMBOL,      // push :
    clkwk_PUSH_CONSTANT,    // push <>
    clkwk_DISPATCH,         // disp sel #
    clkwk_DISPATCH_STACK,   // disps #
    clkwk_RETURN,           // return
    clkwk_SHUTDOWN,         // end
};

typedef struct assembled_binary assembled_binary;
typedef struct assembler assembler;

//assembled_binary* assembler_assemble_str(struct str* input);
assembled_binary* assembler_assemble_cstr(char* input, uint64_t length, struct clockwork_vm* vm);
assembler* assembler_init(struct clockwork_vm* vm);
void assembler_noop(assembler* ar);
void assembler_pop(assembler* ar);
void assembler_nil(assembler* ar);
void assembler_true(assembler* ar);
void assembler_false(assembler* ar);
void assembler_self(assembler* ar);
void assembler_super(assembler* ar);
void assembler_return(assembler* ar);
void assembler_end(assembler* ar);
void assembler_pushInt(assembler* ar, int64_t value);
void assembler_pushNumber(assembler* ar, double value);
void assembler_pushSymbol(assembler* ar, char* sym);
void assembler_pushString(assembler* ar, char* sym);
void assembler_jump(assembler* ar, uint64_t loc);
void assembler_jumpTrue(assembler* ar, uint64_t loc);
void assembler_jumpFalse(assembler* ar, uint64_t loc);
void assembler_dispatch(assembler* ar, char* sel, unsigned char args);
void assembler_pushLocal(assembler* ar, char* sym);
void assembler_setLocal(assembler* ar, char* sym);
void assembler_popToLocal(assembler* ar, char* sym);
void assembler_pushClockwork(assembler *ar);

uint64_t assembled_binary_size(assembled_binary* asm_bin);
char* assembled_binary_data(assembled_binary* asm_bin);
void assembled_binary_dealloc(assembled_binary* asm_bin, struct clockwork_vm* vm);

#warning THESE ARE IN THE WRONG PLACE!!!!
void assembler_run_instruction(instruction*, struct clockwork_vm*);
void assembler_run_block(block*, struct clockwork_vm*);
