
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
#include "object_table.h"
#include "block.h"
#include "assembler.h"
#include "nil.h"
#include "true_false.h"
#include "primitive_table.h"
#include "integer.h"
#include "array.h"
#include "frame.h"

#include <stdlib.h>
#include <stdio.h>

#pragma mark Clockwork

struct clockwork_vm
{
    class* isa;
    object* super;
    object_table* ivars;

    uint64_t pc;
    stack* stack;
    primitive_table* locals;
    primitive_table* constants;
    object* currentSelf;

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

clockwork_vm* vm_init(void)
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

void vm_dealloc(clockwork_vm* vm)
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

void vm_print(clockwork_vm* vm, str* string)
{
    printf("%.*s", str_length(string, vm), str_raw_bytes(string, vm));
}

void vm_println(clockwork_vm* vm, str* string)
{
    vm_print(vm, string);
    printf("\n");
}

void vm_popPrintln(clockwork_vm* vm)
{
    str* string = (str*)vm_pop(vm);
    vm_println(vm, string);
}

void vm_pushClockwork(clockwork_vm* vm)
{
    stack_push(vm->stack, (object*)vm);
}

object* vm_currentSelf(clockwork_vm* vm)
{
    return vm->currentSelf;
}

#pragma mark - MEMORY MANAGEMENT

void* vm_allocate(clockwork_vm* vm, uint64_t bytes)
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

void vm_free(clockwork_vm* vm, object* obj)
{
    vm_freeSize(vm, obj, object_size(obj));
}

void vm_freeSize(clockwork_vm* vm, void* memory, uint64_t bytes)
{
    vm->allocatedMemory -= bytes;
    free(memory);

    printf("Was %llu. Freeing %llu bytes. Total %llu\n", vm->allocatedMemory + bytes, bytes, vm->allocatedMemory);
}

#pragma mark - PROGRAM COUNTER

void vm_goto(clockwork_vm* vm, uint64_t location)
{
    vm->pc = location;
}

void vm_gotoIfFalse(clockwork_vm* vm, uint64_t location)
{
//    object* obj = stack_pop(vm->stack);
//    if (object_is_false(obj))
//    {
//        vm->pc = location;
//    }
}

void vm_gotoIfTrue(clockwork_vm* vm, uint64_t location)
{
//    object* obj = stack_pop(vm->stack);
//    if (object_is_true(obj))
//    {
//        vm->pc = location;
//    }
}

#pragma mark - PUSH / POP

void vm_push(clockwork_vm* vm, object* obj)
{
#warning TODO: This was trashing the instanceMethods table in pushed classes. Need to rethink this.
//    object_retain(obj, vm);
    stack_push(vm->stack, obj);
}

object* vm_pop(clockwork_vm* vm)
{
#warning LEAK?
    object* obj = stack_pop(vm->stack);
    return obj;
}

void vm_pushNil(clockwork_vm* vm)
{
    stack_push(vm->stack, vm->nilObject);
}

void vm_pushTrue(clockwork_vm* vm)
{
    stack_push(vm->stack, vm->trueObject);
}

void vm_pushFalse(clockwork_vm* vm)
{
    stack_push(vm->stack, vm->falseObject);
}

#pragma mark - LOCALS

void vm_setLocal(clockwork_vm* vm, char* local)
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

void vm_popToLocal(clockwork_vm* vm, char* local)
{
    object* obj = stack_pop(vm->stack);
    if (obj == NULL)
    {
        obj = vm->nilObject;
    }
    primitive_table_set(vm->locals, vm, local, obj);
}

void vm_pushLocal(clockwork_vm* vm, char* local)
{
    object* obj = primitive_table_get(vm->locals, vm, local);
    if (obj == NULL)
    {
        obj = vm->nilObject;
    }
    stack_push(vm->stack, obj);
}

object* vm_getLocal(clockwork_vm* vm, char* local)
{
    return primitive_table_get(vm->locals, vm, local);
}

#pragma mark - IVARS

void vm_setIvar(clockwork_vm* vm, char* ivar)
{
    object* obj = stack_pop(vm->stack);
    object_setIvar(vm->currentSelf, vm, ivar, obj);
    stack_push(vm->stack, obj);
}

void vm_pushIvar(clockwork_vm* vm, char* ivar)
{
    object* obj = object_getIvar(vm->currentSelf, vm, ivar);
    stack_push(vm->stack, obj);
}

#pragma mark - SELF AND SUPER

void vm_pushSelf(clockwork_vm* vm)
{
    stack_push(vm->stack, vm->currentSelf);
}

void vm_pushSuper(clockwork_vm* vm)
{
    stack_push(vm->stack, object_super(vm->currentSelf, vm));
}

#pragma mark - CONSTANT ACCESS

void vm_pushConst(clockwork_vm* vm, char* cnst)
{
    object* value = primitive_table_get(vm->constants, vm, cnst);
    if (value)
    {
        vm_push(vm, value);
    }
    else
    {
#warning THROW EXCEPTION!
    }
}

void vm_setConst(clockwork_vm* vm, char* cnst)
{
    object* value = vm_pop(vm);
    if (value)
    {
        primitive_table_set(vm->constants, vm, cnst, value);
    }
    else
    {
#warning THROW EXCEPTION!
    }
}

struct object* vm_getConstant(clockwork_vm* vm, char* name)
{
    object* c = primitive_table_get(vm->constants, vm, name);
    return c;
}

#pragma mark - CLASSES

void vm_openClass(clockwork_vm* vm, char* newName, char* superName)
{
    class* newClass = class_init(vm, newName, superName);
    primitive_table_set(vm->constants, vm, newName, (object*)newClass);
    vm_push(vm, (object*)newClass);
}

#pragma mark - DISPATCH

void vm_forward(clockwork_vm* vm, object* target, char* message, object** args, uint8_t arg_count)
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

void vm_dispatch(clockwork_vm* vm, char* selector, uint8_t arg_count)
{
    if (stack_count(vm->stack) < arg_count + 1)
    {
        printf("Not enough objects on the stack to satisfy target and %d arguments for selector %s!\n", arg_count, selector);
        return;
    }

    object* args[arg_count];
    for (uint8_t i = 0; i < arg_count; i++)
	{
		args[i] = vm_pop(vm);
	}
    object* target = vm_pop(vm);
    if (!target)
    {
        printf("Could not find target on stack... wtf?");
    }
    block* m = object_findMethod(target, vm, selector);
    vm->frameStack.frames[vm->frameStack.idx++].frameSelf = vm->currentSelf;
    if (m == NULL)
    {
        vm_forward(vm, target, selector, args, arg_count);
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

void vm_return(clockwork_vm* vm)
{
    vm->currentSelf = vm->frameStack.frames[--vm->frameStack.idx].frameSelf;
}

#pragma mark - HELPERS

void vm_makeStringCstr(clockwork_vm* vm, const char* const string)
{
    str* s = str_init(vm, string);
    vm_push(vm, (object*)s);
}
