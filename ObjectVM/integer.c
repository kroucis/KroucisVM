//
//  integer.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-24.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "integer.h"

#include "object.h"
#include "class.h"
#include "vm.h"
#include "primitive_table.h"
#include "block.h"
#include "str.h"

#include "clkwk_debug.h"

#include <stdio.h>

static char* c_IntegerClassName = "Integer";

#pragma mark Numeric

struct numeric
{
    struct object_header header;        /* 40 */
                                    /* = 40 */
};

#pragma mark Integer

struct integer
{
    struct object_header header;        /* 40 */

    int64_t value;                      /* 8 */
                                    /* = 48 */
};

/*
 - add:
 - sub:
 - mul:
 - div:
 - lessThan:
 - greaterThan:
 - description:
 */

#pragma mark - Bound Methods

static void integer_add_native(object* self, clockwork_vm* vm)
{
    object* other = clkwk_getLocal(vm, 0);
    if (other && object_isMemberOfClass_native(other, (class*)clkwk_getConstant(vm, c_IntegerClassName)))
    {
        integer* other_i = (integer*)other;
        integer* self_i = (integer*)self;
        int64_t result = integer_toInt64(self_i, vm) + integer_toInt64(other_i, vm);
        object* result_obj = (object*)integer_init(vm, result);
        clkwk_push(vm, result_obj);
    }
    else
    {
        printf("SHIT WENT WRONG!");
#warning THROW EXCEPTION!
    }

    clkwk_return(vm);
}

static void integer_sub_native(object* self, clockwork_vm* vm)
{
    object* other = clkwk_getLocal(vm, 0);
    if (other && object_isMemberOfClass_native(other, (class*)clkwk_getConstant(vm, c_IntegerClassName)))
    {
        integer* other_i = (integer*)other;
        integer* self_i = (integer*)self;
        int64_t result = integer_toInt64(self_i, vm) - integer_toInt64(other_i, vm);
        object* result_obj = (object*)integer_init(vm, result);
        clkwk_push(vm, result_obj);
    }
    else
    {
#warning THROW EXCEPTION!
    }

    clkwk_return(vm);
}

static void integer_mul_native(object* self, clockwork_vm* vm)
{
    object* other = clkwk_getLocal(vm, 0);
    if (other && object_isMemberOfClass_native(other, (class*)clkwk_getConstant(vm, c_IntegerClassName)))
    {
        integer* other_i = (integer*)other;
        integer* self_i = (integer*)self;
        int64_t result = integer_toInt64(self_i, vm) * integer_toInt64(other_i, vm);
        object* result_obj = (object*)integer_init(vm, result);
        clkwk_push(vm, result_obj);
    }
    else
    {
#warning THROW EXCEPTION!
    }

    clkwk_return(vm);
}

static void integer_div_native(object* self, clockwork_vm* vm)
{
    object* other = clkwk_getLocal(vm, 0);
    if (other && object_isMemberOfClass_native(other, (class*)clkwk_getConstant(vm, c_IntegerClassName)))
    {
        integer* other_i = (integer*)other;
        integer* self_i = (integer*)self;
        int64_t result = integer_toInt64(self_i, vm) / integer_toInt64(other_i, vm);
        object* result_obj = (object*)integer_init(vm, result);
        clkwk_push(vm, result_obj);
    }
    else
    {
#warning THROW EXCEPTION!
    }

    clkwk_return(vm);
}

static void integer_lessThan_native(object* self, clockwork_vm* vm)
{
    object* other = clkwk_getLocal(vm, 0);
    if (other && object_isMemberOfClass_native(other, (class*)clkwk_getConstant(vm, c_IntegerClassName)))
    {
        integer* other_i = (integer*)other;
        integer* self_i = (integer*)self;
        if (integer_toInt64(self_i, vm) < integer_toInt64(other_i, vm))
        {
            clkwk_pushTrue(vm);
        }
        else
        {
            clkwk_pushFalse(vm);
        }
    }
    else
    {
#warning THROW EXCEPTION!
    }

    clkwk_return(vm);
}

static void integer_greaterThan_native(object* self, clockwork_vm* vm)
{
    object* other = clkwk_getLocal(vm, 0);
    if (other && object_isMemberOfClass_native(other, (class*)clkwk_getConstant(vm, c_IntegerClassName)))
    {
        integer* other_i = (integer*)other;
        integer* self_i = (integer*)self;
        if (integer_toInt64(self_i, vm) > integer_toInt64(other_i, vm))
        {
            clkwk_pushTrue(vm);
        }
        else
        {
            clkwk_pushFalse(vm);
        }
    }
    else
    {
#warning THROW EXCEPTION!
    }

    clkwk_return(vm);
}

static void integer_description_native(object* self, clockwork_vm* vm)
{
    integer* self_i = (integer*)self;
    int64_t value = integer_toInt64(self_i, vm);
    char buffer[(value / 10) + 1];
    sprintf(buffer, "%lld", value);
    clkwk_makeStringCstr(vm, buffer);
    clkwk_return(vm);
}

#pragma mark - Native Methods

class* numeric_class(clockwork_vm* vm)
{
    class* numericClass = class_init(vm, "Numeric", "Object");

#ifdef CLKWK_PRINT_SIZES
    CLKWK_DBGPRNT("Numeric: %lu\n", sizeof(numeric));
#endif

    return numericClass;
}

class* integer_class(clockwork_vm* vm)
{
    class* integerClass = class_init(vm, "Integer", "Numeric");

    // Instance Methods
    {
        class_addInstanceMethod(integerClass, vm, "add:", block_init_native(vm, 1, 0, &integer_add_native));
        class_addInstanceMethod(integerClass, vm, "sub:", block_init_native(vm, 1, 0, &integer_sub_native));
        class_addInstanceMethod(integerClass, vm, "mul:", block_init_native(vm, 1, 0, &integer_mul_native));
        class_addInstanceMethod(integerClass, vm, "div:", block_init_native(vm, 1, 0, &integer_div_native));
        class_addInstanceMethod(integerClass, vm, "lessThan:", block_init_native(vm, 1, 0, &integer_lessThan_native));
        class_addInstanceMethod(integerClass, vm, "greaterThan:", block_init_native(vm, 1, 0, &integer_greaterThan_native));
        class_addInstanceMethod(integerClass, vm, "description", block_init_native(vm, 0, 0, &integer_description_native));
    }

#ifdef CLKWK_PRINT_SIZES
    CLKWK_DBGPRNT("Integer: %lu\n", sizeof(integer));
#endif
    
    return integerClass;
}

integer* integer_init(clockwork_vm* vm, int64_t i)
{
    object* objSuper = object_init(vm);
    object* numericSuper = object_create_super(vm, objSuper, (class*)clkwk_getConstant(vm, "Numeric"), sizeof(numeric));
    integer* intObj = (integer*)object_create_super(vm, numericSuper, (class*)clkwk_getConstant(vm, c_IntegerClassName), sizeof(integer));
    intObj->value = i;
    intObj->header.size = sizeof(integer);
    return intObj;
}

int64_t integer_toInt64(integer* instance, clockwork_vm* vm)
{
    return instance->value;
}
