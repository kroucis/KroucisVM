//
//  block.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-17.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include "instruction.h"

struct clockwork_vm;
struct object;
struct ast;
struct class;

typedef struct local_scope local_scope;

typedef void(*native_block)(struct object*, struct clockwork_vm*);

struct class* block_class(struct clockwork_vm*);
uint32_t block_instance_size();

typedef struct block block;

local_scope* local_scope_init(struct clockwork_vm*);
void local_scope_dealloc(local_scope*, struct clockwork_vm*);
void local_scope_addLocal(local_scope*, struct clockwork_vm*, char*);
uint8_t local_scope_count(local_scope*, struct clockwork_vm*);
char* local_scope_localAt(local_scope*, struct clockwork_vm*, uint8_t);

block* block_init_native(struct clockwork_vm*, local_scope*, native_block);
block* block_initIseq(struct clockwork_vm*, local_scope*, instruction_sequence*);
block* block_initAST(struct clockwork_vm*, local_scope*, struct ast*);

void block_dealloc(block*, struct clockwork_vm*);
instruction_sequence* block_instructions(block*, struct clockwork_vm*);
native_block block_native(block*, struct clockwork_vm*);
local_scope* block_locals(block*, struct clockwork_vm*);
