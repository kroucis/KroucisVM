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

#include "clkwk_debug.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

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
    struct object_header header;

    object** contents;
    uint64_t count;
    uint64_t capacity;
};

#pragma mark - Bound Methods

static void array_alloc_native(object* klass, clockwork_vm* vm)
{
    clkwk_push(vm, (object*)array_init(vm));
}

static void array_dealloc_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    uint64_t count = array_count(ary, vm);
    if (count > 0)
    {
        for (uint64_t i = 0; i < count; i++)
        {
            clkwk_push(vm, ary->contents[i]);
            clkwk_dispatch(vm, "release", 0);
            clkwk_pop(vm);
        }

        clkwk_free(vm, ary->contents);
    }
    clkwk_pushSuper(vm);
    clkwk_dispatch(vm, "dealloc", 0);
    clkwk_free(vm, instance);
    clkwk_pop(vm);
    clkwk_pushNil(vm);
    clkwk_return(vm);
}

static void array_count_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    clkwk_push(vm, (object*)integer_init(vm, array_count(ary, vm)));
    clkwk_return(vm);
}

static void array_objectAtIndex_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    object* obj = clkwk_getLocal(vm, 0);
    integer* idx = (integer*)obj;
    if (!object_isMemberOfClass_native(obj, (class*)clkwk_getConstant(vm, "Integer")))     // Not super sure if I actually want type coercion.
    {
        clkwk_push(vm, obj);
        clkwk_dispatch(vm, "toInt", 0);
        idx = (integer*)clkwk_pop(vm);
#warning CONFIRM idx IS ACTUALLY AN INTEGER?
    }
    object* result = array_objectAtIndex(ary, vm, integer_toInt64(idx, vm));
    if (result)
    {
        clkwk_push(vm, result);
    }
    else
    {
        clkwk_pushNil(vm);
    }
    clkwk_return(vm);
}

static void array_add_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    object* obj = clkwk_getLocal(vm, 0);
    array_add(ary, vm, obj);
    clkwk_push(vm, instance);
    clkwk_return(vm);
}

static void array_remove_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    object* obj = clkwk_getLocal(vm, 0);
    boolean removed = No;
    for (uint64_t i = 0; i < array_count(ary, vm); i++)
	{
		clkwk_push(vm, array_objectAtIndex(ary, vm, i));
        clkwk_push(vm, obj);
        clkwk_dispatch(vm, "isEqual:", 1);
        if (object_isTrue(clkwk_pop(vm), vm))
        {
            array_remove(ary, vm, obj);
            removed = Yes;
        }
	}

    if (removed)
    {
        clkwk_push(vm, obj);
    }
    else
    {
        clkwk_pushNil(vm);
    }
    clkwk_return(vm);
}

static void array_isEmpty_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    if (array_count(ary, vm) == 0)
    {
        clkwk_pushTrue(vm);
    }
    else
    {
        clkwk_pushFalse(vm);
    }
    clkwk_return(vm);
}

static void array_contains_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    object* obj = clkwk_getLocal(vm, 0);
    boolean found = No;
    for (int i = 0; i < array_count(ary, vm); i++)
	{
		clkwk_push(vm, array_objectAtIndex(ary, vm, i));
        clkwk_push(vm, obj);
        clkwk_dispatch(vm, "isEqual:", 1);
        if (object_isTrue(clkwk_pop(vm), vm))
        {
            found = Yes;
            break;
        }
	}

    if (found)
    {
        clkwk_pushTrue(vm);
    }
    else
    {
        clkwk_pushFalse(vm);
    }
    clkwk_return(vm);
}

static void array_indexOf_native(object* instance, clockwork_vm* vm)
{
    array* ary = (array*)instance;
    object* obj = clkwk_getLocal(vm, 0);
    for (uint64_t i = 0; i < array_count(ary, vm); i++)
	{
		clkwk_push(vm, array_objectAtIndex(ary, vm, i));
        clkwk_push(vm, obj);
        clkwk_dispatch(vm, "isEqual:", 1);
        if (object_isTrue(clkwk_pop(vm), vm))
        {
            clkwk_push(vm, (object*)integer_init(vm, i));
            return;
        }
	}

    clkwk_pushNil(vm);
    clkwk_return(vm);
}

#pragma mark - Native Methods

class* array_class(clockwork_vm* vm)
{
    class* arrayClass = class_init(vm, "Array", "Object");

    class_addClassMethod(arrayClass, vm, "alloc", block_init_native(vm, 0, 0, &array_alloc_native));
    class_addInstanceMethod(arrayClass, vm, "dealloc", block_init_native(vm, 0, 0, &array_dealloc_native));
    class_addInstanceMethod(arrayClass, vm, "count", block_init_native(vm, 0, 0, &array_count_native));

    {
        class_addInstanceMethod(arrayClass, vm, "objectAtIndex:", block_init_native(vm, 1, 0, &array_objectAtIndex_native));
    }

    {
        class_addInstanceMethod(arrayClass, vm, "add:", block_init_native(vm, 1, 0, &array_add_native));
    }

    {
        class_addInstanceMethod(arrayClass, vm, "remove:", block_init_native(vm, 1, 0, &array_remove_native));
    }
    
    {
        class_addInstanceMethod(arrayClass, vm, "contains:", block_init_native(vm, 1, 0, &array_contains_native));
    }

    {
        class_addInstanceMethod(arrayClass, vm, "indexOf:", block_init_native(vm, 1, 0, &array_indexOf_native));
    }

    {
        class_addInstanceMethod(arrayClass, vm, "isEmpty", block_init_native(vm, 0, 0, &array_isEmpty_native));
    }

#ifdef CLKWK_PRINT_SIZES
    CLKWK_DBGPRNT("Array: %lu\n", sizeof(array));
#endif

    return arrayClass;
}

array* array_init(clockwork_vm* vm)
{
    assert(vm);

    object* arraySuper = object_init(vm);
    array* ary = (array*)object_create_super(vm, arraySuper, (class*)clkwk_getConstant(vm, "Array"), sizeof(array));
    ary->header.size = sizeof(array);
    return ary;
}

array* array_initWithObjects(clockwork_vm* vm, object** objects, uint64_t count)
{
    assert(vm);
    assert(objects);

    array* arrayInit = array_init(vm);
    for (uint64_t i = 0; i < count; i++)
	{
		array_add(arrayInit, vm, objects[i]);
	}

    return arrayInit;
}

uint64_t array_count(array* ary, clockwork_vm* vm)
{
    assert(ary);
    assert(vm);

    return ary->count;
}

object* array_objectAtIndex(array* ary, clockwork_vm* vm, uint64_t index)
{
    assert(ary);
    assert(vm);

    if (index < ary->count)
    {
        return ary->contents[index];
    }
    else
    {
#ifdef CLKWK_DEBUG
        printf("ARRAY INDEX %llu OUT OF RANGE %llu", index, ary->count);
#endif
        return NULL;
    }
}

void array_add(array* ary, clockwork_vm* vm, object* obj)
{
    assert(ary);
    assert(vm);
    // OK to add nil?

    if (ary->contents == NULL)
    {
        ary->contents = clkwk_allocate(vm, sizeof(object*) * c_array_baseStorage);
        ary->capacity = c_array_baseStorage;
    }
    else if (ary->count + 1 > ary->capacity)
    {
#warning GROW ARRAY
    }

    clkwk_push(vm, obj);
    clkwk_dispatch(vm, "retain", 0);
    ary->contents[ary->count++] = clkwk_pop(vm);
}

void array_remove(array* ary, clockwork_vm* vm, object* obj)
{
    assert(ary);
    assert(vm);

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
    assert(ary);
    assert(vm);

    if (idx >= ary->count)
    {
        return;
    }

    clkwk_push(vm, ary->contents[idx]);
    clkwk_dispatch(vm, "release", 0);
    clkwk_pop(vm);

    for (uint64_t i = idx + 1; i < ary->count; i++)
	{
		ary->contents[i - 1] = ary->contents[i];
	}

    ary->count--;

#warning SHRINK contents?
}
