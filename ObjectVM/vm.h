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
struct assembled_binary;
struct symbol;

typedef struct clockwork_vm clockwork_vm;
typedef uint8_t local_index;

/**
 *  Allocate and initialize a new ClockworkVM instance.
 *  @return A new clockwork_vm instance or NULL if an error occurred.
 */
clockwork_vm* clkwk_init(void);

/**
 *  Deallocate the given clockwork_vm instance. This sends -release to the root object in the VM
 *  instance, ideally triggering the dealloc chain for all remaining objects within the VM.
 *
 *  @param vm The VM instance to deallocate.
 */
void clkwk_dealloc(clockwork_vm* vm);

// --- MISCELLANEOUS ---
/**
 *  Print the given string object to the VM's output (stdout by default).
 *
 *  @param vm The VM instance to work with.
 *  @param string The string to print.
 */
void clkwk_print(clockwork_vm* vm, struct str* string);

/**
 *  Print the given string object followed by a newline char to the VM's output (stdout by default).
 *
 *  BEFORE  [ ... ]
 *  AFTER   [ ... ]
 *
 *  @param vm The VM instance to work with.
 *  @param string The string to print.
 */
void clkwk_println(clockwork_vm* vm, struct str* string);

/**
 *  Pop the first object from the stack and pass it to clkwk_println(2).
 *
 *  BEFORE  [ obj, ... ]
 *  AFTER   [ ... ]
 *
 *  @param vm The VM instance to work with.
 */
void clkwk_popPrintln(clockwork_vm* vm);

/**
 *  Push the given clockwork_vm instance onto its own stack.
 *
 *  BEFORE  [ ... ]
 *  AFTER   [ vm, ... ]
 *
 *  @param vm The VM instance to work with.
 */
void clkwk_pushClockwork(clockwork_vm* vm);

/**
 *  Get the current self object in the given ClockworkVM instance.
 *
 *  BEFORE  [ ... ]
 *  AFTER   [ ... ]
 *
 *  @param vm The VM instance to work with.
 *
 *  @return The current self object.
 */
struct object* clkwk_currentSelf(clockwork_vm* vm);

// --- MEMORY MANAGEMENT ---
/**
 *  Ask the given ClockworkVM instance to allocate and manage a chunk of memory.
 *
 *  BEFORE  [ ... ]
 *  AFTER   [ ... ]
 *
 *  @param vm The VM instance to work with.
 *  @param bytes Number of bytes to allocate and track.
 *
 *  @return An allocated chunk that is at least <bytes> in size.
 */
void* clkwk_allocate(clockwork_vm* vm, uint64_t bytes);

/**
 *  Deallocates the memory backing <obj>.
 *
 *  BEFORE  [ ... ]
 *  AFTER   [ ... ]
 *
 *  @note This does not call -release or -dealloc on any of the object's members, it simply returns
 *      the memory to the free pool.
 *  @param vm The VM instance to work with.
 *  @param obj The object to free.
 */
void clkwk_free(clockwork_vm* vm, void* obj);

// --- EXECUTION ---
void clkwk_runBinary(clockwork_vm* vm, struct assembled_binary* binary);

// --- PROGRAM COUNTER ---
void clkwk_jump(clockwork_vm* vm, uint64_t location);
void clkwk_jumpIfFalse(clockwork_vm* vm, uint64_t location);
void clkwk_jumpIfTrue(clockwork_vm* vm, uint64_t location);

// --- PUSH / POP ---
void clkwk_push(clockwork_vm* vm, struct object* obj);
struct object* clkwk_pop(clockwork_vm* vm);
void clkwk_pushNil(clockwork_vm* vm);
void clkwk_pushTrue(clockwork_vm* vm);
void clkwk_pushFalse(clockwork_vm* vm);

// --- LOCALS ---
void clkwk_setLocal(clockwork_vm* vm, local_index local);
void clkwk_popToLocal(clockwork_vm* vm, local_index local);
void clkwk_pushLocal(clockwork_vm* vm, local_index local);
struct object* clkwk_getLocal(clockwork_vm* vm, local_index local);

// --- IVARS ---
void clkwk_setIvar(clockwork_vm* vm, struct symbol* ivar);
void clkwk_pushIvar(clockwork_vm* vm, struct symbol* ivar);

// --- SELF AND SUPER ---
void clkwk_pushSelf(clockwork_vm* vm);
void clkwk_pushSuper(clockwork_vm* vm);

// --- CONSTANTS ---
void clkwk_pushConst(clockwork_vm* vm, const char* name);
void clkwk_setConst(clockwork_vm* vm, const char* name);
struct object* clkwk_getConstant(clockwork_vm* vm, const char* name);

// --- CLASSES ---
struct class* clkwk_openClass(clockwork_vm* vm, char*, char*);
//void clkwk_openClassWithMixins(clockwork_vm*, char*, char*, char**, uint8_t);

// --- BLOCKS ---
void clkwk_openBlock(clockwork_vm* vm);
void clkwk_closeBlock(clockwork_vm* vm);

// --- DISPATCH ---
void clkwk_dispatch(clockwork_vm* vm, char* selector, uint8_t arg_count);
void clkwk_return(clockwork_vm* vm);

// --- HELPERS ---
void clkwk_makeStringCstr(clockwork_vm* vm, const char* const string);
struct symbol* clkwk_getSymbolCstr(clockwork_vm* vm, const char* const sym_str);
struct symbol* clkwk_getSymbol(clockwork_vm* vm, struct str* sym);
uint8_t clkwk_stackLimit(clockwork_vm* vm);
