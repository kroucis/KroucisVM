
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

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#pragma mark Clockwork

struct clockwork_vm
{
    class* isa;
    object* super;
    dictionary* ivars;

    uint64_t pc;
    stack* stack;
    primitive_table* locals;
    primitive_table* constants;
    object* currentSelf;

    assembled_binary* binary;

    // Special object cache?
    object* nilObject;
    object* trueObject;
    object* falseObject;

    frame_stack frameStack;

    // TODO: Internal memory for the VM?
//    void* memoryPage;
    uint64_t allocatedMemory;
};

#pragma mark - Bound Methods

#pragma mark - Native Methods

class* clockwork_class(clockwork_vm* vm)
{
    class* clockworkClass = class_init(vm, "Clockwork", "Object");
    return clockworkClass;
}

clockwork_vm* clkwk_init(void)
{
    clockwork_vm* vm = (clockwork_vm*)calloc(1, sizeof(clockwork_vm));
    vm->stack = stack_init();
    vm->locals = primitive_table_init(vm, 10);
    vm->constants = primitive_table_init(vm, 16);

//    vm->memoryPage = malloc(kVMMemorySize);

    class* objectClass = object_class(vm);
    primitive_table_set(vm->constants, vm, class_name(objectClass, vm), (object*)objectClass);

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

    vm->isa = clockworkClass;

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
#warning FIX THIS: NEED TO FREE ALL ALLOCATED MEMORY (CLASSES, LOCALS, AND ALL OBJECT GRAPHS)

//    primitive_table_each(vm->locals, vm, dealloc_primitive_table_contents);
//    primitive_table_dealloc(vm->locals, vm, No);
//
//    primitive_table_each(vm->constants, vm, dealloc_primitive_table_contents);
//    primitive_table_dealloc(vm->constants, vm, No);
    free(vm);
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

void* clkwk_allocate(clockwork_vm* vm, uint64_t bytes)
{
    void* value = calloc(1, bytes);
    if (!value)
    {
        printf("calloc FAILED TO RETURN VIABLE MEMORY!");
    }

    vm->allocatedMemory += bytes;

    printf("Was %llu. Allocating %llu bytes. Total %llu\n", vm->allocatedMemory - bytes, bytes, vm->allocatedMemory);

    return value;
}

void clkwk_free(clockwork_vm* vm, object* obj)
{
    clkwk_freeSize(vm, obj, object_size(obj));
}

void clkwk_freeSize(clockwork_vm* vm, void* memory, uint64_t bytes)
{
    vm->allocatedMemory -= bytes;
    free(memory);

    printf("Was %llu. Freeing %llu bytes. Total %llu\n", vm->allocatedMemory + bytes, bytes, vm->allocatedMemory);
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
        char* magic_byes = "CLKWK1";
        if (len < strlen(magic_byes))
        {
            printf("ClockworkVM: Incompatible binary.\n");
            return;
        }

        for (int i = 0; i < strlen(magic_byes); i++)
        {
            if (data[i] != magic_byes[i])
            {
                printf("ClockworkVM: Incompatible binary.\n");
                return;
            }
        }

        vm->pc += strlen(magic_byes);
    }

    while (vm->pc < len)
    {
        // Get next opcode
        char op = data[vm->pc++];
        switch (op)
        {
            case clkwk_NOOP:
            {
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
            case clkwk_PUSH_TRUE:
            {
                clkwk_pushTrue(vm);
                break;
            }
            case clkwk_PUSH_FALSE:
            {
                clkwk_pushFalse(vm);
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
            case clkwk_PUSH_INT:
            {
                int64_t i;
                memcpy(&i, &data[vm->pc], sizeof(int64_t));
                vm->pc += sizeof(int64_t);
                integer* intObj = integer_init(vm, i);
                clkwk_push(vm, (object*)intObj);
                break;
            }
            case clkwk_PUSH_NUMBER:
            {
                double d;
                memcpy(&d, &data[vm->pc], sizeof(double));
                vm->pc += sizeof(double);
//                nu
//                clkwk_push(vm, (object*)intObj);
                printf("FLOATING NUMBER NOT IMPLEMENTED YET!\n");
                exit(1);
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

                clkwk_makeStringCstr(vm, string);       // Makes and pushes string literal.
                break;
            }
            case clkwk_JUMP:
            {
                uint64_t loc;
                memcpy(&loc, &data[vm->pc], sizeof(uint64_t));
                vm->pc = loc;
                break;
            }
            case clkwk_JUMP_IF_TRUE:
            {
                object* o = clkwk_pop(vm);
                if (object_isTrue(o, vm))
                {
                    uint64_t loc;
                    memcpy(&loc, &data[vm->pc], sizeof(uint64_t));
                    vm->pc = loc;
                }
                else
                {
                    vm->pc += sizeof(uint64_t);
                }
                break;
            }
            case clkwk_JUMP_IF_FALSE:
            {
                object* o = clkwk_pop(vm);
                if (object_isFalse(o, vm))
                {
                    uint64_t loc;
                    memcpy(&loc, &data[vm->pc], sizeof(uint64_t));
                    vm->pc = loc;
                }
                else
                {
                    vm->pc += sizeof(uint64_t);
                }
                break;
            }
            case clkwk_PUSH_LOCAL:
            {
                unsigned char sym_len = data[vm->pc++];
                char sym[255];
                memcpy(&sym, &data[vm->pc], sym_len);
                vm->pc += sym_len;

                clkwk_pushLocal(vm, sym);
                break;
            }
            case clkwk_SET_LOCAL:
            {
                unsigned char sym_len = data[vm->pc++];
                char sym[255];
                memcpy(&sym, &data[vm->pc], sym_len);
                vm->pc += sym_len;

                clkwk_setLocal(vm, sym);
                break;
            }
            case clkwk_POP_TO_LOCAL:
            {
                unsigned char sym_len = data[vm->pc++];
                char sym[sym_len + 1];
                memcpy(&sym, &data[vm->pc], sym_len);
                sym[sym_len] = '\0';
                vm->pc += sym_len;

                clkwk_popToLocal(vm, sym);
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

                clkwk_dispatch(vm, sym, args);
            }
            case clkwk_RETURN:
            {
                clkwk_return(vm);
                break;
            }
            case clkwk_SHUTDOWN:
            {
                vm->pc = len;
                break;
            }
            default:
            {
                printf("ClockworkVM: UNKNOWN OPCODE %d\n", op);
                break;
            }
        }
    }

    printf("VM TERMINATING\n");
}

#pragma mark - PROGRAM COUNTER

void clkwk_goto(clockwork_vm* vm, uint64_t location)
{
    vm->pc = location;
}

void clkwk_gotoIfFalse(clockwork_vm* vm, uint64_t location)
{
//    object* obj = stack_pop(vm->stack);
//    if (object_is_false(obj))
//    {
//        vm->pc = location;
//    }
}

void clkwk_gotoIfTrue(clockwork_vm* vm, uint64_t location)
{
//    object* obj = stack_pop(vm->stack);
//    if (object_is_true(obj))
//    {
//        vm->pc = location;
//    }
}

#pragma mark - PUSH / POP

void clkwk_push(clockwork_vm* vm, object* obj)
{
#warning TODO: This was trashing the instanceMethods table in pushed classes. Need to rethink this.
//    object_retain(obj, vm);
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

void clkwk_setLocal(clockwork_vm* vm, symbol local)
{
    object* obj = stack_pop(vm->stack);
    if (obj == NULL)
    {
        obj = vm->nilObject;
    }

#warning TODO: GROW LOCALS WHEN NEEDED

    primitive_table_set(vm->locals, vm, local, obj);
    stack_push(vm->stack, obj);
}

void clkwk_popToLocal(clockwork_vm* vm, symbol local)
{
    object* obj = stack_pop(vm->stack);
    if (obj == NULL)
    {
        obj = vm->nilObject;
    }
    primitive_table_set(vm->locals, vm, local, obj);
}

void clkwk_pushLocal(clockwork_vm* vm, symbol local)
{
    object* obj = primitive_table_get(vm->locals, vm, local);
    if (obj == NULL)
    {
        obj = vm->nilObject;
    }
    stack_push(vm->stack, obj);
}

object* clkwk_getLocal(clockwork_vm* vm, symbol local)
{
    return primitive_table_get(vm->locals, vm, local);
}

#pragma mark - IVARS

void clkwk_setIvar(clockwork_vm* vm, symbol ivar)
{
    object* obj = stack_pop(vm->stack);
    object_setIvar(vm->currentSelf, vm, ivar, obj);
    stack_push(vm->stack, obj);
}

void clkwk_pushIvar(clockwork_vm* vm, symbol ivar)
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

void clkwk_pushConst(clockwork_vm* vm, symbol cnst)
{
    object* value = primitive_table_get(vm->constants, vm, cnst);
    if (value)
    {
        clkwk_push(vm, value);
    }
    else
    {
#warning THROW EXCEPTION!
    }
}

void clkwk_setConst(clockwork_vm* vm, symbol cnst)
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

struct object* clkwk_getConstant(clockwork_vm* vm, symbol name)
{
    object* c = primitive_table_get(vm->constants, vm, name);
    return c;
}

#pragma mark - CLASSES

void clkwk_openClass(clockwork_vm* vm, char* newName, char* superName)
{
    class* newClass = class_init(vm, newName, superName);
    primitive_table_set(vm->constants, vm, newName, (object*)newClass);
    clkwk_push(vm, (object*)newClass);
}

#pragma mark - DISPATCH

void clkwk_forward(clockwork_vm* vm, object* target, char* message, object** args, uint8_t arg_count)
{
    block* m = object_findMethod(target, vm, "forwardMessage:withArguments:");
    if (m == NULL)
    {
#warning THROW EXCEPTION
        return;
    }

#warning PACKAGE args INTO ARRAY
    str* messageStr = str_init(vm, message);
    object* argsArray = (object*)array_initWithObjects(vm, args, arg_count);     // REPLACE WITH arg ARRAY OBJECT

    local_scope* locals = block_locals(m, vm);
    if (local_scope_count(locals, vm) != 2)
    {
        printf("Incorrect argument count to forward message %s!\n", message);
        return;
    }
    vm->currentSelf = target;
    primitive_table_set(vm->locals, vm, local_scope_localAt(locals, vm, 0), (object*)messageStr);
    primitive_table_set(vm->locals, vm, local_scope_localAt(locals, vm, 1), argsArray);
    assembler_run_block(m, vm);
}

void clkwk_dispatch(clockwork_vm* vm, char* selector, uint8_t arg_count)
{
    if (stack_count(vm->stack) < arg_count + 1)
    {
        printf("Not enough objects on the stack to satisfy target and %d arguments for selector %s!\n", arg_count, selector);
        return;
    }

    object* args[arg_count];
    for (uint8_t i = 0; i < arg_count; i++)
	{
		args[i] = clkwk_pop(vm);
	}
    object* target = clkwk_pop(vm);
    if (!target)
    {
        printf("Could not find target on stack... wtf?");
    }
    block* m = object_findMethod(target, vm, selector);
    frame* nextFrame = &vm->frameStack.frames[vm->frameStack.idx++];
    nextFrame->frameSelf = vm->currentSelf;
//    nextFrame->returnPC = vm->pc + 1;
    if (m == NULL)
    {
        clkwk_forward(vm, target, selector, args, arg_count);
        return;
    }
    local_scope* locals = block_locals(m, vm);
    if (local_scope_count(locals, vm) != arg_count)
    {
        printf("Missing arguments for selector %s!\n", selector);
        return;
    }
    vm->currentSelf = target;
    for (int i = 0; i < arg_count; i++)
	{
        primitive_table_set(vm->locals, vm, local_scope_localAt(locals, vm, i), args[i]);
	}
    assembler_run_block(m, vm);
}

void clkwk_return(clockwork_vm* vm)
{
    frame* oldFrame = &vm->frameStack.frames[--vm->frameStack.idx];
    vm->currentSelf = oldFrame->frameSelf;
//    vm->pc = oldFrame->returnPC;
}

#pragma mark - HELPERS

void clkwk_makeStringCstr(clockwork_vm* vm, const char* const string)
{
    str* s = str_init(vm, string);
    clkwk_push(vm, (object*)s);
}
