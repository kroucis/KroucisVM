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
uint32_t block_instance_size(void);

typedef struct block block;

// local_scope* local_scope_init(struct clockwork_vm*);
// void local_scope_dealloc(local_scope*, struct clockwork_vm*);
// void local_scope_addLocal(local_scope*, struct clockwork_vm*, char*);
// uint8_t local_scope_count(local_scope*, struct clockwork_vm*);
// char* local_scope_localAt(local_scope*, struct clockwork_vm*, uint8_t);

block* block_init_compiled(struct clockwork_vm* vm, uint8_t localsCount, uint8_t upvalsCount, struct object** upvals, uint64_t pc, struct object* enclosedSelf, struct object* enclosedSuper); // , struct class* declaringClass);  ?
block* block_init_native(struct clockwork_vm* vm, uint8_t localsCount, uint8_t upvalsCount, native_block func);
// block* block_init_compiled(struct clockwork_vm* vm, uint8_t localsCount, uint8_t upvalsCount, uint64_t pc);
// block* block_initAST(struct clockwork_vm*, local_scope*, struct ast*);

void block_dealloc(block* blk, struct clockwork_vm* vm);
instruction_sequence* block_instructions(block* blk, struct clockwork_vm* vm);
native_block block_nativeFunction(block* blk, struct clockwork_vm* vm);
uint8_t block_localsCount(block* blk, struct clockwork_vm* vm);
uint8_t block_upvalsCount(block* blk, struct clockwork_vm* vm);
uint64_t block_pcLocation(block* blk, struct clockwork_vm* vm);
