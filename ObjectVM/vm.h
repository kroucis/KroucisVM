//
//  vm.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-10.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include <inttypes.h>

struct object;
struct str;

typedef struct clockwork_vm clockwork_vm;

clockwork_vm* vm_init(void);
void vm_dealloc(clockwork_vm*);

// MISCELLANEOUS
void vm_print(clockwork_vm*, struct str*);
void vm_println(clockwork_vm*, struct str*);
void vm_popPrintln(clockwork_vm*);
void vm_pushClockwork(clockwork_vm* vm);

struct object* vm_currentSelf(clockwork_vm*);

// MEMORY MANAGEMENT
void* vm_allocate(clockwork_vm*, uint64_t);
void vm_free(clockwork_vm*, struct object*);
void vm_freeSize(clockwork_vm* vm, void* memory, uint64_t bytes);

// INSTRUCTIONS

// PROGRAM COUNTER  -- SHOULD THIS BE HERE? THIS IS A FUNCTION OF THE ASSEMBLER, RIGHT?
void vm_goto(clockwork_vm* vm, uint64_t location);
void vm_gotoIfFalse(clockwork_vm* vm, uint64_t location);
void vm_gotoIfTrue(clockwork_vm* vm, uint64_t location);

// PUSH / POP
void vm_push(clockwork_vm* vm, struct object* obj);
struct object* vm_pop(clockwork_vm* vm);
void vm_pushNil(clockwork_vm* vm);
void vm_pushTrue(clockwork_vm* vm);
void vm_pushFalse(clockwork_vm* vm);

// LOCALS
void vm_setLocal(clockwork_vm* vm, char* local);
void vm_popToLocal(clockwork_vm* vm, char* local);
void vm_pushLocal(clockwork_vm* vm, char* local);
struct object* vm_getLocal(clockwork_vm* vm, char* local);

// IVARS
void vm_setIvar(clockwork_vm* vm, char* ivar);
void vm_pushIvar(clockwork_vm* vm, char* ivar);

// SELF AND SUPER
void vm_pushSelf(clockwork_vm* vm);
void vm_pushSuper(clockwork_vm* vm);

// CONSTANTS
void vm_pushConst(clockwork_vm*, char*);
void vm_setConst(clockwork_vm*, char*);
struct object* vm_getConstant(clockwork_vm*, char*);

// CLASSES
void vm_openClass(clockwork_vm*, char*, char*);
//void vm_openClassWithMixins(clockwork_vm*, char*, char*, char**, uint8_t);

// BLOCKS
void vm_openBlock(clockwork_vm*);
void vm_closeBlock(clockwork_vm*);

// DISPATCH
void vm_dispatch(clockwork_vm* vm, char* selector, uint8_t arg_count);
void vm_return(clockwork_vm* vm);

// HELPERS
void vm_makeStringCstr(clockwork_vm* vm, const char* const string);
