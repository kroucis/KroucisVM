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
        case VM_NOOP:
        {
            break;
        }
        case VM_GOTO:
        {
            uint64_t location = cstr_to_uint64(inst->params[0]);
            vm_goto(vm, location);
            break;
        }
        case VM_GOTO_IF_FALSE:
        {
            uint64_t location = cstr_to_uint64(inst->params[0]);
            vm_gotoIfFalse(vm, location);
            break;
        }
        case VM_GOTO_IF_TRUE:
        {
            uint64_t location = cstr_to_uint64(inst->params[0]);
            vm_gotoIfTrue(vm, location);
            break;
        }
        case VM_POP:
        {
            vm_pop(vm);
            break;
        }
        case VM_PUSH_NIL:
        {
            vm_pushNil(vm);
            break;
        }
        case VM_PUSH_FALSE:
        {
            vm_pushFalse(vm);
            break;
        }
        case VM_PUSH_TRUE:
        {
            vm_pushTrue(vm);
            break;
        }
        case VM_PUSH_LOCAL:
        {
            vm_pushLocal(vm, inst->params[0]);
            break;
        }
        case VM_SET_LOCAL:
        {
            vm_setLocal(vm, inst->params[0]);
            break;
        }
        case VM_POP_TO_LOCAL:
        {
            vm_popToLocal(vm, inst->params[0]);
            break;
        }
        case VM_PUSH_SELF:
        {
            vm_pushSelf(vm);
            break;
        }
        case VM_PUSH_SUPER:
        {
            vm_pushSuper(vm);
            break;
        }
        case VM_PUSH_IVAR:
        {
            vm_pushIvar(vm, inst->params[0]);
            break;
        }
        case VM_SET_IVAR:
        {
            vm_setIvar(vm, inst->params[0]);
            break;
        }
        case VM_PRINT:
        {
            vm_popPrintln(vm);
            break;
        }
        case VM_PUSH_STRING:
        {
            str* s = str_init(vm, inst->params[0]);
            vm_push(vm, (object*)s);
            break;
        }
        case VM_PUSH_INT:
        {
            integer* i = integer_init(vm, cstr_to_int64(inst->params[0]));
            vm_push(vm, (object*)i);
            break;
        }
        case VM_PUSH_NUMBER:
        {
            break;
        }
        case VM_PUSH_CONSTANT:
        {
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
            native(vm_currentSelf(vm), vm);
        }
        else
        {
#warning TODO: Throw and exception?
            printf("NATIVE block WAS NULL!");
        }
    }
}
