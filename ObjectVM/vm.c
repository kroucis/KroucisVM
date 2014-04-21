
//
//  vm.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-10.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "vm.h"

#include "class.h"
#include "object.h"
#include "str.h"
#include "stack.h"
#include "dictionary.h"
#include "block.h"
#include "assembler.h"
#include "nil.h"
#include "true_false.h"
#include "primitive_table.h"
#include "integer.h"
#include "array.h"
#include "frame.h"
#include "assembler.h"

#include "symbols.h"

#include "memory_manager.h"

#include "clkwk_debug.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>

#define MEM_ALLOC

#pragma mark Clockwork

//static const uint8_t c_VMLocalsLimit = 10;
static const uint8_t c_VMStackLimit = 64;

struct clockwork_vm
{
    struct object_header header;            /* 40 */

    vm_address pc;                            /* 8 */
    stack* stack;                           /* 8 */
    primitive_table* constants;             /* 8 */
    object* currentSelf;                    /* 8 */

    assembled_binary* binary;               /* 8 */

    symbol_table* symbols;                  /* 8 */

    // Special object cache?
    object* nilObject;                      /* 8 */
    object* trueObject;                     /* 8 */
    object* falseObject;                    /* 8 */

    frame_stack frameStack;                 /* 4616 */

    // TODO: Internal memory for the VM?
//    void* memoryPage;
//    uint64_t allocatedMemory;

                                        /* = 4728 */
};

#pragma mark - Private Helpers

static frame* _clkwk_current_frame(clockwork_vm* vm)
{
    return &vm->frameStack.frames[vm->frameStack.idx];
}

static frame* _clkwk_push_frame(clockwork_vm* vm)
{
    if (vm->frameStack.idx + 1 >= clkwk_stackLimit(vm))
    {
        return NULL;
    }

    return &vm->frameStack.frames[vm->frameStack.idx++];
}

static frame* _clkwk_pop_frame(clockwork_vm* vm)
{
    if (vm->frameStack.idx == 0)
    {
        return NULL;
    }

    return &vm->frameStack.frames[--vm->frameStack.idx];
}

static void _clkwk_run_block(clockwork_vm* vm, block* blk)
{
    if (blk)
    {
        native_block n_blk = block_nativeFunction(blk, vm);
        uint64_t c_pc = block_pcLocation(blk, vm);
        if (n_blk)
        {
            n_blk(clkwk_currentSelf(vm), vm);
        }
        else if (c_pc)
        {
            vm->pc = block_pcLocation(blk, vm);
        }
        else
        {
            printf("INVALID BLOCK!\n");
        }
    }
    else
    {
        printf("NULL BLOCK!\n");
    }
}

#pragma mark - Bound Methods
void clkwk_openClass_native(object* slf, clockwork_vm* vm)
{
    symbol* name = (symbol*)clkwk_getLocal(vm, 0);
    class* klass = clkwk_openClass(vm, symbol_cstr(name), "Object");
    clkwk_push(vm, (object*)klass);
    clkwk_return(vm);
}

#pragma mark - Native Methods

class* clockwork_class(clockwork_vm* vm)
{
    class* clockworkClass = class_init(vm, "Clockwork", "Object");

#ifdef CLKWK_PRINT_SIZES
    CLKWK_DBGPRNT("Clockwork: %lu\n", sizeof(clockwork_vm));
#endif

    return clockworkClass;
}

clockwork_vm* clkwk_init(void)
{
    clockwork_vm* vm = MEM_MALLOC(sizeof(clockwork_vm));
    vm->stack = stack_init();
    vm->constants = primitive_table_init(vm, 16);
    vm->symbols = symbol_table_init(vm);

    class* objectClass = object_class(vm);
    primitive_table_set(vm->constants, vm, class_name(objectClass, vm), (object*)objectClass);

    class* symbolClass = symbol_class(vm);
    primitive_table_set(vm->constants, vm, class_name(symbolClass, vm), (object*)symbolClass);

    class* strClass = string_class(vm);
    primitive_table_set(vm->constants, vm, class_name(strClass, vm), (object*)strClass);

    class* blockClass = block_class(vm);
    primitive_table_set(vm->constants, vm, class_name(blockClass, vm), (object*)blockClass);

    class* arrayClass = array_class(vm);
    primitive_table_set(vm->constants, vm, class_name(arrayClass, vm), (object*)arrayClass);

    class* nilClass = nil_class(vm);
    primitive_table_set(vm->constants, vm, class_name(nilClass, vm), (object*)nilClass);

    vm->nilObject = nil_instance(vm);  // ?

    class* trueClass = true_class(vm);
    primitive_table_set(vm->constants, vm, class_name(trueClass, vm), (object*)trueClass);

    vm->trueObject = true_instance(vm);

    class* falseClass = false_class(vm);
    primitive_table_set(vm->constants, vm, class_name(falseClass, vm), (object*)falseClass);

    vm->falseObject = false_instance(vm);

    class* numericClass = numeric_class(vm);
    primitive_table_set(vm->constants, vm, class_name(numericClass, vm), (object*)numericClass);

    class* intClass = integer_class(vm);
    primitive_table_set(vm->constants, vm, class_name(intClass, vm), (object*)intClass);

    class* clockworkClass = clockwork_class(vm);
    primitive_table_set(vm->constants, vm, class_name(clockworkClass, vm), (object*)vm);

    object* clkSuper = object_init(vm);
    vm->header.isa = clockworkClass;
    vm->header.super = clkSuper;
    vm->header.size = sizeof(clockwork_vm);
    vm->header.extra = NULL;

    vm->currentSelf = (object*)vm;

    {
        class_addInstanceMethod(clockworkClass, vm, "openClass:", block_init_native(vm, 1, 0, &clkwk_openClass_native));
    }

    return vm;
}

void clkwk_dealloc(clockwork_vm* vm)
{
    for (int i = 0; i < stack_count(vm->stack); i++)
	{
		object* o = stack_pop(vm->stack);
        object_dealloc(o, vm);
	}
    stack_dealloc(vm->stack);
    symbol_table_dealloc(vm->symbols, vm);
#warning FIX THIS: NEED TO FREE ALL ALLOCATED MEMORY (CLASSES, LOCALS, AND ALL OBJECT GRAPHS)

//    primitive_table_each(vm->locals, vm, dealloc_primitive_table_contents);
//    primitive_table_dealloc(vm->locals, vm, Yes);

//    primitive_table_each(vm->constants, vm, dealloc_primitive_table_contents);
//    primitive_table_dealloc(vm->constants, vm, Yes);
    MEM_FREE(vm);
}

#pragma mark - MISCELLANEOUS

void clkwk_print(clockwork_vm* vm, str* string)
{
    printf("%.*s", str_length(string, vm), str_raw_bytes(string, vm));
}

void clkwk_println(clockwork_vm* vm, str* string)
{
    clkwk_print(vm, string);
    printf("\n");
}

void clkwk_popPrintln(clockwork_vm* vm)
{
    str* string = (str*)clkwk_pop(vm);
    clkwk_println(vm, string);
}

void clkwk_pushClockwork(clockwork_vm* vm)
{
    stack_push(vm->stack, (object*)vm);
}

object* clkwk_currentSelf(clockwork_vm* vm)
{
    return vm->currentSelf;
}

#pragma mark - MEMORY MANAGEMENT

void* clkwk_allocate(clockwork_vm* vm, vm_size bytes)
{
    void* value = MEM_MALLOC(bytes);
    assert(value);

//    void* value = calloc(1, bytes);
//    if (!value)
//    {
//        printf("clkwk_allocate FAILED TO RETURN VIABLE MEMORY!");
//    }

//    vm->allocatedMemory += bytes;

//    printf("Was %llu. Allocating %llu bytes. Total %llu\n", vm->allocatedMemory - bytes, bytes, vm->allocatedMemory);

    return value;
}

void clkwk_free(clockwork_vm* vm, void* obj)
{
    MEM_FREE(obj);
}

#pragma mark - EXECUTION
void clkwk_runBinary(clockwork_vm* vm, assembled_binary* binary)
{
    vm->binary = binary;
    vm->pc = 0;
    char* data = assembled_binary_data(vm->binary);
    uint64_t len = assembled_binary_size(vm->binary);

    // Verify binary signature
    {
        char* magic_bytes = "CLKWK1";
        size_t magic_len = strlen(magic_bytes);
        assert(len > magic_len);
        if (len < magic_len)
        {
            printf("ClockworkVM: Incompatible binary.\n");
            return;
        }

        for (int i = 0; i < magic_len; i++)
        {
            if (data[i] != magic_bytes[i])
            {
                printf("ClockworkVM: Incompatible binary.\n");
                return;
            }
        }

        vm->pc += magic_len;
    }

    while (vm->pc < len)
    {
        // Get next opcode
        char op = data[vm->pc++];
        switch (op)
        {
            case clkwk_NOOP:
            {
                CLKWK_DBGPRNT("NOOP\n");
                break;
            }
            case clkwk_POP:
            {
                CLKWK_DBGPRNT("POP\n");
                clkwk_pop(vm);
                break;
            }
            case clkwk_PUSH_NIL:
            {
                CLKWK_DBGPRNT("PUSH_NIL\n");
                clkwk_pushNil(vm);
                break;
            }
            case clkwk_PUSH_TRUE:
            {
                CLKWK_DBGPRNT("PUSH_TRUE\n");
                clkwk_pushTrue(vm);
                break;
            }
            case clkwk_PUSH_FALSE:
            {
                CLKWK_DBGPRNT("PUSH_FALSE\n");
                clkwk_pushFalse(vm);
                break;
            }
            case clkwk_PUSH_SELF:
            {
                CLKWK_DBGPRNT("PUSH_SELF\n");
                clkwk_pushSelf(vm);
                break;
            }
            case clkwk_PUSH_SUPER:
            {
                CLKWK_DBGPRNT("PUSH_SUPER\n");
                clkwk_pushSuper(vm);
                break;
            }
            case clkwk_PUSH_INT:
            {
                int64_t i;
                memcpy(&i, &data[vm->pc], sizeof(int64_t));
                vm->pc += sizeof(int64_t);
                integer* intObj = integer_init(vm, i);

                CLKWK_DBGPRNT("PUSH_INT %lld\n", integer_toInt64(intObj, vm));

                clkwk_push(vm, (object*)intObj);
                break;
            }
            case clkwk_PUSH_NUMBER:
            {
                double d;
                memcpy(&d, &data[vm->pc], sizeof(double));
                vm->pc += sizeof(double);
                printf("FLOATING NUMBER NOT IMPLEMENTED YET!\n");
                exit(EXIT_FAILURE);
                break;
            }
            case clkwk_PUSH_STRING:
            {
                uint64_t len;
                memcpy(&len, &data[vm->pc], sizeof(uint64_t));
                vm->pc += sizeof(uint64_t);

                char string[len + 1];
                memcpy(string, &data[vm->pc], len);
                string[len] = '\0';
                vm->pc += len;

                CLKWK_DBGPRNT("PUSH_STRING %lld '%s'\n", len, string);

                clkwk_makeStringCstr(vm, string);       // Makes and pushes string literal.
                break;
            }
            case clkwk_PUSH_SYMBOL:
            {
                uint64_t len;
                memcpy(&len, &data[vm->pc], sizeof(uint64_t));
                vm->pc += sizeof(uint64_t);

                char sym[len + 1];
                memcpy(sym, &data[vm->pc], len);
                sym[len] = '\0';
                vm->pc += len;

                CLKWK_DBGPRNT("PUSH_SYMBOL %lld '%s'\n", len, sym);

                symbol* theSymbol = symbol_table_get(vm->symbols, sym, vm);
                clkwk_push(vm, (object*)theSymbol);
                break;
            }
            case clkwk_PUSH_CONSTANT:
            {
                uint64_t len;
                memcpy(&len, &data[vm->pc], sizeof(uint64_t));
                vm->pc += sizeof(uint64_t);

                char constant[len + 1];
                memcpy(constant, &data[vm->pc], len);
                constant[len] = '\0';
                vm->pc += len;

                CLKWK_DBGPRNT("PUSH_CONSTANT %lld '%s'\n", len, constant);

                clkwk_pushConst(vm, constant);
                break;
            }
            case clkwk_JUMP:
            {
                vm_address loc;
                memcpy(&loc, &data[vm->pc], sizeof(vm_address));

                CLKWK_DBGPRNT("JUMP %lld\n", loc);

                clkwk_jump(vm, loc);
                break;
            }
            case clkwk_JUMP_IF_TRUE:
            {
                object* o = clkwk_pop(vm);
                if (object_isTrue(o, vm))
                {
                    vm_address loc;
                    memcpy(&loc, &data[vm->pc], sizeof(vm_address));
                    clkwk_jump(vm, loc);
                }
                else
                {
                    clkwk_jump(vm, vm->pc + sizeof(vm_address));
                }
                break;
            }
            case clkwk_JUMP_IF_FALSE:
            {
                object* o = clkwk_pop(vm);
                if (object_isFalse(o, vm))
                {
                    vm_address loc;
                    memcpy(&loc, &data[vm->pc], sizeof(vm_address));
                    clkwk_jump(vm, loc);
                }
                else
                {
                    clkwk_jump(vm, vm->pc + sizeof(vm_address));
                }
                break;
            }
            case clkwk_PUSH_LOCAL:
            {
                local_index idx;
                uint8_t idx_sz = sizeof(local_index);
                memcpy(&idx, &data[vm->pc], idx_sz);
                vm->pc += idx_sz;

                CLKWK_DBGPRNT("PUSH_LOCAL %d\n", idx);

                clkwk_pushLocal(vm, idx);
                break;
            }
            case clkwk_SET_LOCAL:
            {
                local_index idx;
                uint8_t idx_sz = sizeof(local_index);
                memcpy(&idx, &data[vm->pc], idx_sz);
                vm->pc += idx_sz;

                clkwk_setLocal(vm, idx);
                break;
            }
            case clkwk_POP_TO_LOCAL:
            {
                local_index idx;
                uint8_t idx_sz = sizeof(local_index);
                memcpy(&idx, &data[vm->pc], idx_sz);
                vm->pc += idx_sz;

                clkwk_popToLocal(vm, idx);
                break;
            }
            case clkwk_DISPATCH:
            {
                uint8_t args = data[vm->pc++];
                uint8_t sym_len = data[vm->pc++];
                char sym[255];
                memcpy(sym, &data[vm->pc], sym_len);
                sym[sym_len] = '\0';
                vm->pc += sym_len;

                CLKWK_DBGPRNT("DISPATCH %s %d\n", sym, args);

                clkwk_dispatch(vm, sym, args);
                break;
            }
            case clkwk_PUSH_CLOCKWORK:
            {
                CLKWK_DBGPRNT("PUSH_CLOCKWORK\n");
                clkwk_pushClockwork(vm);
                break;
            }
            case clkwk_RETURN:
            {
                CLKWK_DBGPRNT("RETURN\n");
                clkwk_return(vm);
                break;
            }
            case clkwk_SHUTDOWN:
            {
                CLKWK_DBGPRNT("SHUTDOWN\n");
                vm->pc = len;
                break;
            }
            default:
            {
                printf("ClockworkVM: UNKNOWN OPCODE %c\n", op);
                break;
            }
        }
    }

    CLKWK_DBGPRNT("VM TERMINATING\n");
}

#pragma mark - PROGRAM COUNTER

void clkwk_jump(clockwork_vm* vm, vm_address location)
{
    vm->pc = location;
}

void clkwk_jumpIfFalse(clockwork_vm* vm, vm_address location)
{
    object* obj = stack_pop(vm->stack);
    if (object_isFalse(obj, vm))
    {
        clkwk_jump(vm, location);
    }
}

void clkwk_jumpIfTrue(clockwork_vm* vm, vm_address location)
{
    object* obj = stack_pop(vm->stack);
    if (object_isTrue(obj, vm))
    {
        clkwk_jump(vm, location);
    }
}

#pragma mark - PUSH / POP

void clkwk_push(clockwork_vm* vm, object* obj)
{
    object_retain(obj, vm);
    stack_push(vm->stack, obj);
}

object* clkwk_pop(clockwork_vm* vm)
{
#warning LEAK?
    object* obj = stack_pop(vm->stack);
    return obj;
}

void clkwk_pushNil(clockwork_vm* vm)
{
    stack_push(vm->stack, vm->nilObject);
}

void clkwk_pushTrue(clockwork_vm* vm)
{
    stack_push(vm->stack, vm->trueObject);
}

void clkwk_pushFalse(clockwork_vm* vm)
{
    stack_push(vm->stack, vm->falseObject);
}

#pragma mark - LOCALS

void clkwk_setLocal(clockwork_vm* vm, local_index local)
{
    assert(local < c_LocalsLimit);
    object* obj = stack_pop(vm->stack);
    if (obj == NULL)
    {
        obj = vm->nilObject;
    }

#warning TODO: GROW LOCALS WHEN NEEDED

    _clkwk_current_frame(vm)->locals[local] = obj;
    stack_push(vm->stack, obj);
}

void clkwk_popToLocal(clockwork_vm* vm, local_index local)
{
    assert(local < c_LocalsLimit);
    object* obj = stack_pop(vm->stack);
    if (obj == NULL)
    {
        obj = vm->nilObject;
    }
    _clkwk_current_frame(vm)->locals[local] = obj;
}

void clkwk_pushLocal(clockwork_vm* vm, local_index local)
{
    assert(local < c_LocalsLimit);
    object* obj = _clkwk_current_frame(vm)->locals[local];
    if (obj == NULL)
    {
        obj = vm->nilObject;
    }
    stack_push(vm->stack, obj);
}

object* clkwk_getLocal(clockwork_vm* vm, local_index local)
{
    assert(local < c_LocalsLimit);
    return _clkwk_current_frame(vm)->locals[local];
}

#pragma mark - IVARS

void clkwk_setIvar(clockwork_vm* vm, symbol* ivar)
{
    object* obj = stack_pop(vm->stack);
    object_setIvar(vm->currentSelf, vm, ivar, obj);
    stack_push(vm->stack, obj);
}

void clkwk_pushIvar(clockwork_vm* vm, symbol* ivar)
{
    object* obj = object_getIvar(vm->currentSelf, vm, ivar);
    stack_push(vm->stack, obj);
}

#pragma mark - SELF AND SUPER

void clkwk_pushSelf(clockwork_vm* vm)
{
    stack_push(vm->stack, vm->currentSelf);
}

void clkwk_pushSuper(clockwork_vm* vm)
{
    stack_push(vm->stack, object_super(vm->currentSelf, vm));
}

#pragma mark - CONSTANT ACCESS

void clkwk_pushConst(clockwork_vm* vm, const char* cnst)
{
    object* value = primitive_table_get(vm->constants, vm, cnst);
    if (value)
    {
        clkwk_push(vm, value);
    }
    else
    {
#warning THROW EXCEPTION!
        printf("Could not find constant '%s'!\n", cnst);
        exit(EXIT_FAILURE);
    }
}

void clkwk_setConst(clockwork_vm* vm, const char* cnst)
{
    object* value = clkwk_pop(vm);
    if (value)
    {
        primitive_table_set(vm->constants, vm, cnst, value);
    }
    else
    {
#warning THROW EXCEPTION!
    }
}

struct object* clkwk_getConstant(clockwork_vm* vm, const char* name)
{
    object* c = primitive_table_get(vm->constants, vm, name);
    return c;
}

#pragma mark - CLASSES

class* clkwk_openClass(clockwork_vm* vm, char* newName, char* superName)
{
    class* newClass = class_init(vm, newName, superName);
    primitive_table_set(vm->constants, vm, newName, (object*)newClass);
    return newClass;
}

#pragma mark - DISPATCH

void clkwk_forward(clockwork_vm* vm, object* target, char* message, object** args, uint8_t arg_count)
{
    block* m = object_findMethod(target, vm, clkwk_getSymbolCstr(vm, "forwardMessage:withArguments:"));
    if (m == NULL)
    {
#warning THROW EXCEPTION
        printf("EXCEPTION: Could not find forwardMessage:withArguments:\n");
        return;
    }

    symbol* messageSym = symbol_table_get(vm->symbols, message, vm);
    object* argsArray = (object*)array_initWithObjects(vm, args, arg_count);

    uint8_t localsCount = block_localsCount(m, vm);
    assert(localsCount == 2);
    if (localsCount != 2)
    {
        printf("Incorrect argument count to forward message %s!\n", message);
        return;
    }
    vm->currentSelf = target;

    frame* cur_frame = _clkwk_current_frame(vm);
    cur_frame->locals[0] = (object*)messageSym;
    cur_frame->locals[1] = argsArray;

    _clkwk_run_block(vm, m);
}

void clkwk_dispatch(clockwork_vm* vm, char* selector, uint8_t arg_count)
{
    if (stack_count(vm->stack) < arg_count + 1)
    {
        printf("Not enough objects on the stack to satisfy target and %d arguments for selector %s!\n", arg_count, selector);
        return;
    }

    object* args[arg_count];
    for (local_index i = 0; i < arg_count; i++)
	{
		args[(arg_count - 1) - i] = clkwk_pop(vm);
	}

    object* target = clkwk_pop(vm);
    assert(target);
    if (!target)
    {
        printf("Could not find target on stack... wtf?\n");
    }
    block* m = object_findMethod(target, vm, clkwk_getSymbolCstr(vm, selector));
    
    frame* nextFrame = _clkwk_push_frame(vm);
    if (nextFrame == NULL)
    {
        printf("Clockwork: Stack overflow.\n");
        return;
    }
    nextFrame->frameSelf = vm->currentSelf;
    nextFrame->returnPC = vm->pc;
    if (m == NULL)
    {
        clkwk_forward(vm, target, selector, args, arg_count);
        return;
    }
    uint8_t locals_count = block_localsCount(m, vm);
    assert(locals_count == arg_count);
    if (locals_count != arg_count)
    {
        printf("Missing arguments for selector %s!\n", selector);
        return;
    }
    vm->currentSelf = target;
    for (local_index i = 0; i < arg_count; i++)
	{
        frame* cur_frame = _clkwk_current_frame(vm);
        cur_frame->locals[i] = args[i];
	}

    _clkwk_run_block(vm, m);
}

void clkwk_return(clockwork_vm* vm)
{
    frame* oldFrame = _clkwk_pop_frame(vm);
    vm->currentSelf = oldFrame->frameSelf;
    vm->pc = oldFrame->returnPC;
}

#pragma mark - HELPERS

void clkwk_makeStringCstr(clockwork_vm* vm, const char* const string)
{
    str* s = str_init(vm, string);
    clkwk_push(vm, (object*)s);
}

struct symbol* clkwk_getSymbolCstr(clockwork_vm* vm, const char* const sym_str)
{
    return symbol_table_get(vm->symbols, sym_str, vm);
}

struct symbol* clkwk_getSymbol(clockwork_vm* vm, struct str* sym)
{
    size_t len = str_length(sym, vm);
    char temp[len + 1];
    memcpy(temp, str_raw_bytes(sym, vm), len);
    temp[len] = '\0';

    return clkwk_getSymbolCstr(vm, temp);
}

uint8_t clkwk_stackLimit(clockwork_vm* vm)
{
    return c_VMStackLimit;
}
