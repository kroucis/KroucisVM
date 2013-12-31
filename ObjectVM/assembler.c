//
//  assembler.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-16.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "assembler.h"

#include "vm.h"
#include "str.h"
#include "object.h"
#include "integer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t str_to_uint64(str* string, clockwork_vm* vm)
{
    uint64_t value = 0;
    const char* const data = str_raw_bytes(string, vm);
    for (int i = 0; i < str_length(string, vm); i++)
	{
		value = (value * 10) + (data[i] - '0');
	}

    return value;
}

static uint64_t cstr_to_uint64(char* string)
{
    uint64_t value = 0;
    for (int i = 0; i < strlen(string); i++)
	{
		value = (value * 10) + (string[i] - '0');
	}

    return value;
}

static int64_t cstr_to_int64(char* string)
{
   return atoll(string);
}

void assembler_run_instruction(instruction* inst, clockwork_vm* vm)
{
    switch (inst->op)
    {
        case clkwk_NOOP:
        {
            break;
        }
        case clkwk_GOTO:
        {
            uint64_t location = cstr_to_uint64(inst->params[0]);
            clkwk_goto(vm, location);
            break;
        }
        case clkwk_GOTO_IF_FALSE:
        {
            uint64_t location = cstr_to_uint64(inst->params[0]);
            clkwk_gotoIfFalse(vm, location);
            break;
        }
        case clkwk_GOTO_IF_TRUE:
        {
            uint64_t location = cstr_to_uint64(inst->params[0]);
            clkwk_gotoIfTrue(vm, location);
            break;
        }
        case clkwk_POP:
        {
            clkwk_pop(vm);
            break;
        }
        case clkwk_PUSH_NIL:
        {
            clkwk_pushNil(vm);
            break;
        }
        case clkwk_PUSH_FALSE:
        {
            clkwk_pushFalse(vm);
            break;
        }
        case clkwk_PUSH_TRUE:
        {
            clkwk_pushTrue(vm);
            break;
        }
        case clkwk_PUSH_LOCAL:
        {
            clkwk_pushLocal(vm, inst->params[0]);
            break;
        }
        case clkwk_SET_LOCAL:
        {
            clkwk_setLocal(vm, inst->params[0]);
            break;
        }
        case clkwk_POP_TO_LOCAL:
        {
            clkwk_popToLocal(vm, inst->params[0]);
            break;
        }
        case clkwk_PUSH_SELF:
        {
            clkwk_pushSelf(vm);
            break;
        }
        case clkwk_PUSH_SUPER:
        {
            clkwk_pushSuper(vm);
            break;
        }
        case clkwk_PUSH_IVAR:
        {
            clkwk_pushIvar(vm, inst->params[0]);
            break;
        }
        case clkwk_SET_IVAR:
        {
            clkwk_setIvar(vm, inst->params[0]);
            break;
        }
        case clkwk_PRINT:
        {
            clkwk_popPrintln(vm);
            break;
        }
        case clkwk_PUSH_STRING:
        {
            str* s = str_init(vm, inst->params[0]);
            clkwk_push(vm, (object*)s);
            break;
        }
        case clkwk_PUSH_INT:
        {
            integer* i = integer_init(vm, cstr_to_int64(inst->params[0]));
            clkwk_push(vm, (object*)i);
            break;
        }
        case clkwk_PUSH_NUMBER:
        {
#warning IMPLEMENT
            break;
        }
        case clkwk_PUSH_CONSTANT:
        {
#warning IMPLEMENT
            break;
        }
        case clkwk_RETURN:
        {
            clkwk_return(vm);
            break;
        }
        default:
        {
            break;
        }
    }
}

void assembler_run_block(block* block, struct clockwork_vm* vm)
{
    instruction_sequence* iseq = block_instructions(block, vm);
    if (iseq != NULL)
    {
        for (int i = 0; i < iseq->inst_count; i++)
        {
            assembler_run_instruction(&iseq->instructions[i], vm);
        }
    }
    else
    {
        native_block native = block_native(block, vm);
        if (native != NULL)
        {
            native(clkwk_currentSelf(vm), vm);
        }
        else
        {
#warning TODO: Throw and exception?
            printf("NATIVE block WAS NULL!");
        }
    }
}
