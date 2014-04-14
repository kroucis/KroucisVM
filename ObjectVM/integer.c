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
        clkwk_return(vm);
    }
    else
    {
        printf("SHIT WENT WRONG!");
#warning THROW EXCEPTION!
    }
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
        clkwk_return(vm);
    }
    else
    {
#warning THROW EXCEPTION!
    }
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
        clkwk_return(vm);
    }
    else
    {
#warning THROW EXCEPTION!
    }
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
        clkwk_return(vm);
    }
    else
    {
#warning THROW EXCEPTION!
    }
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
        clkwk_return(vm);
    }
    else
    {
#warning THROW EXCEPTION!
    }
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
        clkwk_return(vm);
    }
    else
    {
#warning THROW EXCEPTION!
    }
}

static void integer_description(object* self, clockwork_vm* vm)
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
    printf("Numeric: %lu\n", sizeof(numeric));
#endif

//    {
//        block* isNilMethodNative = block_init_native(vm, (struct local_scope){ .count = 0 }, &nil_is_nil_native);
//        class_addClassMethod(nilClass, vm, "isNil", isNilMethodNative);
//        class_addInstanceMethod(nilClass, vm, "isNil", isNilMethodNative);
//    }
//
//    {
//        block* isTrueMethodNative = block_init_native(vm, (struct local_scope){ .count = 0 }, &nil_is_true_native);
//        class_addClassMethod(nilClass, vm, "isTrue", isTrueMethodNative);
//        class_addInstanceMethod(nilClass, vm, "isTrue", isTrueMethodNative);
//    }
//
//    {
//        block* isFalseMethodNative = block_init_native(vm, (struct local_scope){ .count = 0 }, &nil_is_false_native);
//        class_addClassMethod(nilClass, vm, "isFalse", isFalseMethodNative);
//        class_addInstanceMethod(nilClass, vm, "isFalse", isFalseMethodNative);
//    }

    return numericClass;
}

class* integer_class(clockwork_vm* vm)
{
    class* integerClass = class_init(vm, "Integer", "Numeric");

    {
        block* addMethod = block_init_native(vm, 1, 0, &integer_add_native);
        class_addInstanceMethod(integerClass, vm, "add:", addMethod);
    }

    {
        block* subMethod = block_init_native(vm, 1, 0, &integer_sub_native);
        class_addInstanceMethod(integerClass, vm, "sub:", subMethod);
    }

    {
        block* mulMethod = block_init_native(vm, 1, 0, &integer_mul_native);
        class_addInstanceMethod(integerClass, vm, "mul:", mulMethod);
    }

    {
        block* divMethod = block_init_native(vm, 1, 0, &integer_div_native);
        class_addInstanceMethod(integerClass, vm, "div:", divMethod);
    }

    {
        block* ltMethod = block_init_native(vm, 1, 0, &integer_lessThan_native);
        class_addInstanceMethod(integerClass, vm, "lessThan:", ltMethod);
    }

    {
        block* gtMethod = block_init_native(vm, 1, 0, &integer_greaterThan_native);
        class_addInstanceMethod(integerClass, vm, "greaterThan:", gtMethod);
    }

    {
        block* to_s_Method = block_init_native(vm, 0, 0, &integer_description);
        class_addInstanceMethod(integerClass, vm, "description", to_s_Method);
    }

#ifdef CLKWK_PRINT_SIZES
    printf("Integer: %lu\n", sizeof(integer));
#endif

    //
    //    {
    //        block* isTrueMethodNative = block_init_native(vm, (struct local_scope){ .count = 0 }, &nil_is_true_native);
    //        class_addClassMethod(nilClass, vm, "isTrue", isTrueMethodNative);
    //        class_addInstanceMethod(nilClass, vm, "isTrue", isTrueMethodNative);
    //    }
    //
    //    {
    //        block* isFalseMethodNative = block_init_native(vm, (struct local_scope){ .count = 0 }, &nil_is_false_native);
    //        class_addClassMethod(nilClass, vm, "isFalse", isFalseMethodNative);
    //        class_addInstanceMethod(nilClass, vm, "isFalse", isFalseMethodNative);
    //    }
    
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
