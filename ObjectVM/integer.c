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

#pragma mark Numeric

struct numeric
{
    class* isa;
    object* super;
    primitive_table* ivars;
};

#pragma mark Integer

struct integer
{
    class* isa;
    object* super;
    primitive_table* ivars;
    uint32_t size;

    int64_t data;
};

#pragma mark - Bound Methods

static void integer_add_native(object* self, clockwork_vm* vm)
{
    object* other = clkwk_getLocal(vm, "other");
    if (other && object_isMemberOfClass_native(other, (class*)clkwk_getConstant(vm, "Integer")))
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
#warning THROW EXCEPTION!
    }
}

static void integer_sub_native(object* self, clockwork_vm* vm)
{
    object* other = clkwk_getLocal(vm, "other");
    if (other && object_isMemberOfClass_native(other, (class*)clkwk_getConstant(vm, "Integer")))
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
    object* other = clkwk_getLocal(vm, "other");
    if (other && object_isMemberOfClass_native(other, (class*)clkwk_getConstant(vm, "Integer")))
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
    object* other = clkwk_getLocal(vm, "other");
    if (other && object_isMemberOfClass_native(other, (class*)clkwk_getConstant(vm, "Integer")))
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
    object* other = clkwk_getLocal(vm, "other");
    if (other && object_isMemberOfClass_native(other, (class*)clkwk_getConstant(vm, "Integer")))
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
    object* other = clkwk_getLocal(vm, "other");
    if (other && object_isMemberOfClass_native(other, (class*)clkwk_getConstant(vm, "Integer")))
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

#pragma mark - Native Methods

class* numeric_class(clockwork_vm* vm)
{
    class* numericClass = class_init(vm, "Numeric", "Object");

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
        local_scope* add_ls = local_scope_init(vm);
        local_scope_addLocal(add_ls, vm, "other");
        block* addMethod = block_init_native(vm, add_ls, &integer_add_native);
        class_addInstanceMethod(integerClass, vm, "add:", addMethod);
    }

    {
        local_scope* sub_ls = local_scope_init(vm);
        local_scope_addLocal(sub_ls, vm, "other");
        block* subMethod = block_init_native(vm, sub_ls, &integer_sub_native);
        class_addInstanceMethod(integerClass, vm, "sub:", subMethod);
    }

    {
        local_scope* mul_ls = local_scope_init(vm);
        local_scope_addLocal(mul_ls, vm, "other");
        block* mulMethod = block_init_native(vm, mul_ls, &integer_mul_native);
        class_addInstanceMethod(integerClass, vm, "mul:", mulMethod);
    }

    {
        local_scope* div_ls = local_scope_init(vm);
        local_scope_addLocal(div_ls, vm, "other");
        block* divMethod = block_init_native(vm, div_ls, &integer_div_native);
        class_addInstanceMethod(integerClass, vm, "div:", divMethod);
    }

    {
        local_scope* lt_ls = local_scope_init(vm);
        local_scope_addLocal(lt_ls, vm, "other");
        block* ltMethod = block_init_native(vm, lt_ls, &integer_lessThan_native);
        class_addInstanceMethod(integerClass, vm, "lessThan:", ltMethod);
    }

    {
        local_scope* gt_ls = local_scope_init(vm);
        local_scope_addLocal(gt_ls, vm, "other");
        block* gtMethod = block_init_native(vm, gt_ls, &integer_greaterThan_native);
        class_addInstanceMethod(integerClass, vm, "greaterThan:", gtMethod);
    }

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
    integer* intObj = (integer*)object_create_super(vm, numericSuper, (class*)clkwk_getConstant(vm, "Integer"), sizeof(integer));
    intObj->data = i;
    intObj->size = sizeof(integer);
    return intObj;
}

int64_t integer_toInt64(integer* instance, clockwork_vm* vm)
{
    return instance->data;
}
