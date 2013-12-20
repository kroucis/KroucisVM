//
//  array.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-12-6.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "array.h"

#include "object.h"
#include "class.h"
#include "vm.h"
#include "block.h"
#include "primitive_table.h"
#include "integer.h"

#include <stdlib.h>
#include <stdio.h>

static uint8_t const c_array_baseStorage = 10;
static uint8_t const c_array_growthFactor = 2;

//struct array_entry
//{
//    object** contents;
//    struct array_entry* next;
//};

#pragma mark Array

struct array
{
    class* isa;
    object* super;
    primitive_table* ivars;
    uint32_t size;

    object** contents;
    uint64_t count;
    uint64_t capacity;
};

#pragma mark - Bound Methods

static void array_alloc_native(object* klass, clockwork_vm* vm)
{
    vm_push(vm, (object*)array_init(vm));
}

static void array_dealloc_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    uint64_t count = array_count(ary, vm);
    if (count > 0)
    {
        for (uint64_t i = 0; i < count; i++)
        {
            vm_push(vm, ary->contents[i]);
            vm_dispatch(vm, "release", 0);
            vm_pop(vm);
        }

        vm_freeSize(vm, ary->contents, sizeof(object*) * count);
    }
    vm_pushSuper(vm);
    vm_dispatch(vm, "dealloc", 0);
    vm_free(vm, instance);
    vm_pop(vm);
    vm_pushNil(vm);
    vm_return(vm);
}

static void array_count_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    vm_push(vm, (object*)integer_init(vm, array_count(ary, vm)));
    vm_return(vm);
}

static void array_objectAtIndex_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    object* obj = vm_getLocal(vm, "idx");
    integer* idx = (integer*)obj;
    if (!object_isMemberOfClass_native(obj, (class*)vm_getConstant(vm, "Integer")))     // Not super sure if I actually want type coercion.
    {
        vm_push(vm, obj);
        vm_dispatch(vm, "toInt", 0);
        idx = (integer*)vm_pop(vm);
#warning CONFIRM idx IS ACTUALLY AN INTEGER?
    }
    object* result = array_objectAtIndex(ary, vm, integer_toInt64(idx, vm));
    if (result)
    {
        vm_push(vm, result);
    }
    else
    {
        vm_pushNil(vm);
    }
    vm_return(vm);
}

static void array_add_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    object* obj = vm_getLocal(vm, "obj");
    array_add(ary, vm, obj);
    vm_push(vm, instance);
    vm_return(vm);
}

static void array_remove_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    object* obj = vm_getLocal(vm, "obj");
    boolean removed = No;
    for (uint64_t i = 0; i < array_count(ary, vm); i++)
	{
		vm_push(vm, array_objectAtIndex(ary, vm, i));
        vm_push(vm, obj);
        vm_dispatch(vm, "isEqual:", 1);
        if (object_isTrue(vm_pop(vm), vm))
        {
            array_remove(ary, vm, obj);
            removed = Yes;
        }
	}

    if (removed)
    {
        vm_push(vm, obj);
    }
    else
    {
        vm_pushNil(vm);
    }
    vm_return(vm);
}

static void array_isEmpty_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    if (array_count(ary, vm) == 0)
    {
        vm_pushTrue(vm);
    }
    else
    {
        vm_pushFalse(vm);
    }
    vm_return(vm);
}

static void array_contains_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    object* obj = vm_getLocal(vm, "obj");
    boolean found = No;
    for (int i = 0; i < array_count(ary, vm); i++)
	{
		vm_push(vm, array_objectAtIndex(ary, vm, i));
        vm_push(vm, obj);
        vm_dispatch(vm, "isEqual:", 1);
        if (object_isTrue(vm_pop(vm), vm))
        {
            found = Yes;
            break;
        }
	}

    if (found)
    {
        vm_pushTrue(vm);
    }
    else
    {
        vm_pushFalse(vm);
    }
    vm_return(vm);
}

static void array_indexOf_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    object* obj = vm_getLocal(vm, "obj");
    for (uint64_t i = 0; i < array_count(ary, vm); i++)
	{
		vm_push(vm, array_objectAtIndex(ary, vm, i));
        vm_push(vm, obj);
        vm_dispatch(vm, "isEqual:", 1);
        if (object_isTrue(vm_pop(vm), vm))
        {
            vm_push(vm, (object*)integer_init(vm, i));
            return;
        }
	}

    vm_pushNil(vm);
    vm_return(vm);
}

#pragma mark - Native Methods

class* array_class(clockwork_vm* vm)
{
    class* arrayClass = class_init(vm, "Array", "Object");

    class_addClassMethod(arrayClass, vm, "alloc", block_init_native(vm, NULL, &array_alloc_native));
    class_addInstanceMethod(arrayClass, vm, "dealloc", block_init_native(vm, NULL, &array_dealloc_native));
    class_addInstanceMethod(arrayClass, vm, "count", block_init_native(vm, NULL, &array_count_native));

    {
        local_scope* oai_ls = local_scope_init(vm);
        local_scope_addLocal(oai_ls, vm, "idx");
        class_addInstanceMethod(arrayClass, vm, "objectAtIndex:", block_init_native(vm, oai_ls, &array_objectAtIndex_native));
    }

    {
        local_scope* a_ls = local_scope_init(vm);
        local_scope_addLocal(a_ls, vm, "obj");
        class_addInstanceMethod(arrayClass, vm, "add:", block_init_native(vm, a_ls, &array_add_native));
    }

    {
        local_scope* r_ls = local_scope_init(vm);
        local_scope_addLocal(r_ls, vm, "obj");
        class_addInstanceMethod(arrayClass, vm, "remove:", block_init_native(vm, r_ls, &array_remove_native));
    }
    
    {
        local_scope* c_ls = local_scope_init(vm);
        local_scope_addLocal(c_ls, vm, "obj");
        class_addInstanceMethod(arrayClass, vm, "contains:", block_init_native(vm, c_ls, &array_contains_native));
    }

    {
        local_scope* io_ls = local_scope_init(vm);
        local_scope_addLocal(io_ls, vm, "obj");
        class_addInstanceMethod(arrayClass, vm, "indexOf:", block_init_native(vm, io_ls, &array_indexOf_native));
    }

    class_addInstanceMethod(arrayClass, vm, "isEmpty", block_init_native(vm, NULL, &array_isEmpty_native));

    return arrayClass;
}

array* array_init(clockwork_vm* vm)
{
    object* arraySuper = object_init(vm);
    array* ary = (array*)object_create_super(vm, arraySuper, (class*)vm_getConstant(vm, "Array"), sizeof(array));
    ary->size = sizeof(array);
    return ary;
}

array* array_initWithObjects(clockwork_vm* vm, object** objects, uint64_t count)
{
    array* arrayInit = array_init(vm);
    for (uint64_t i = 0; i < count; i++)
	{
		array_add(arrayInit, vm, objects[i]);
	}

    return arrayInit;
}

uint64_t array_count(array* ary, clockwork_vm* vm)
{
    return ary->count;
}

object* array_objectAtIndex(array* ary, clockwork_vm* vm, uint64_t index)
{
    if (index < ary->count)
    {
        return ary->contents[index];
    }
    else
    {
#warning THROW EXCEPTION?
        printf("ARRAY INDEX %llu OUT OF RANGE %llu", index, ary->count);
        return NULL;
    }
}

void array_add(array* ary, clockwork_vm* vm, object* obj)
{
    if (ary->contents == NULL)
    {
        ary->contents = vm_allocate(vm, sizeof(object*) * c_array_baseStorage);
        ary->capacity = c_array_baseStorage;
    }
    else if (ary->count + 1 > ary->capacity)
    {
#warning GROW ARRAY
    }

    vm_push(vm, obj);
    vm_dispatch(vm, "retain", 0);
    ary->contents[ary->count++] = vm_pop(vm);
}

void array_remove(array* ary, clockwork_vm* vm, object* obj)
{
    for (uint64_t i = 0; i < ary->count; i++)
	{
		if (ary->contents[i] == obj)
        {
            array_removeAtIndex(ary, vm, i);
        }
	}
}

void array_removeAtIndex(array* ary, clockwork_vm* vm, uint64_t idx)
{
    vm_push(vm, ary->contents[idx]);
    vm_dispatch(vm, "release", 0);
    vm_pop(vm);

    for (uint64_t i = idx + 1; i < ary->count; i++)
	{
		ary->contents[i - 1] = ary->contents[i];
	}

    ary->count--;

#warning SHRINK contents?
}
