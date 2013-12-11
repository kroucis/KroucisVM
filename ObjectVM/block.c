//
//  block.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-17.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "block.h"

#include "instruction.h"
#include "class.h"
#include "object.h"
#include "str.h"
#include "vm.h"
#include "primitive_table.h"

#include <stdlib.h>
#include <memory.h>
#include <inttypes.h>
#include <string.h>

struct local_scope
{
    uint8_t count;
    char** locals;
};

#pragma mark Block

struct block
{
    class* isa;
    object* super;
    primitive_table* ivars;

    local_scope* locals;
    instruction_sequence* instructions;
    native_block nativeFunc;
};

local_scope* local_scope_init(clockwork_vm* vm)
{
    local_scope* ls = vm_allocate(vm, sizeof(local_scope));
    return ls;
}

void local_scope_dealloc(local_scope* scope, clockwork_vm* vm)
{
    if (scope->count > 0)
    {
        for (int i = 0; i < scope->count; i++)
        {
            vm_free(vm, scope->locals[i]);
        }
        vm_free(vm, scope->locals);
    }
    vm_free(vm, scope);
}

void local_scope_addLocal(local_scope* scope, clockwork_vm* vm, char* localName)
{
    if (scope->locals == NULL)
    {
        scope->locals = vm_allocate(vm, sizeof(char*) * 5);
    }

    scope->locals[scope->count] = vm_allocate(vm, sizeof(strlen(localName)));
    strcpy(scope->locals[scope->count], localName);
    scope->count++;
}

uint8_t local_scope_count(local_scope* scope, clockwork_vm* vm)
{
    return scope ? scope->count : 0;
}

char* local_scope_localAt(local_scope* scope, clockwork_vm* vm, uint8_t idx)
{
    if (idx >= scope->count)
    {
        return NULL;
    }

    return scope->locals[idx];
}

#pragma mark - Native Methods

class* block_class(clockwork_vm* vm)
{
    class* block_class = class_init(vm, "Block", "Object");
    return block_class;
}

block* block_initIseq(clockwork_vm* vm, local_scope* locals, instruction_sequence* iseq)
{
    block* m = (block*)vm_allocate(vm, sizeof(block));
    m->locals = locals;
    m->instructions = vm_allocate(vm, sizeof(iseq));
    m->instructions->inst_count = iseq->inst_count;
    m->instructions->instructions = vm_allocate(vm, sizeof(instruction) * iseq->inst_count);
    m->nativeFunc = NULL;
    memcpy(m->instructions->instructions, iseq->instructions, sizeof(instruction) * iseq->inst_count);

    return m;
}

block* block_init_native(clockwork_vm* vm, local_scope* locals, native_block func)
{
    block* m = (block*)vm_allocate(vm, sizeof(block));
    m->locals = locals;
    m->instructions = NULL;
    m->nativeFunc = func;

    return m;
}

void block_dealloc(block* instance, clockwork_vm* vm)
{
    if (instance->locals)
    {
        vm_free(vm, instance->locals);
    }

    if (!instance->nativeFunc)
    {
        vm_free(vm, instance->instructions->instructions);
        vm_free(vm, instance->instructions);
    }

    vm_free(vm, instance);
}

instruction_sequence* block_instructions(block* instance, clockwork_vm* vm)
{
    return instance->instructions;
}

local_scope* block_locals(block* instance, clockwork_vm* vm)
{
    return instance->locals;
}

native_block block_native(block* instance, clockwork_vm* vm)
{
    return instance->nativeFunc;
}
