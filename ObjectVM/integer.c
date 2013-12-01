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

struct numeric
{
    class* isa;
    object* super;
    primitive_table* ivars;
};

struct integer
{
    class* isa;
    object* super;
    primitive_table* ivars;

    int64_t data;
};

static void integer_add_native(object* instance, clockwork_vm* vm)
{
    object* other = vm_getLocal(vm, "other");
    if (other && object_isMemberOfClass_native(other, (class*)vm_getConstant(vm, "Integer")))
    {
        integer* other_i = (integer*)other;
        integer* self_i = (integer*)instance;
        int64_t result = integer_toInt64(self_i, vm) + integer_toInt64(other_i, vm);
        object* result_obj = (object*)integer_init(vm, result);
        vm_push(vm, result_obj);
    }
    else
    {
#warning THROW EXCEPTION!
    }
}

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
    object* numericSuper = object_create_super(vm, objSuper, (class*)vm_getConstant(vm, "Numeric"), sizeof(numeric));
    integer* intObj = (integer*)object_create_super(vm, numericSuper, (class*)vm_getConstant(vm, "Integer"), sizeof(integer));
    intObj->data = i;
    return intObj;
}

int64_t integer_toInt64(integer* instance, clockwork_vm* vm)
{
    return instance->data;
}
