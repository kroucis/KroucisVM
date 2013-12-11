//
//  nil.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-20.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "nil.h"

#include "vm.h"
#include "class.h"
#include "object.h"
#include "str.h"
#include "block.h"
#include "object_table.h"

#include <stdlib.h>

#pragma mark Nil

#pragma mark - Bound Methods

static void nil_isNil_native(object* obj, clockwork_vm* vm)
{
    vm_pushTrue(vm);
    vm_return(vm);
}

static void nil_isTrue_native(object* obj, clockwork_vm* vm)
{
    vm_pushFalse(vm);
    vm_return(vm);
}

static void nil_isFalse_native(object* obj, clockwork_vm* vm)
{
    vm_pushTrue(vm);
    vm_return(vm);
}

static void nil_dealloc_native(object* obj, clockwork_vm* vm)
{
    vm_pushNil(vm);
    vm_return(vm);
}

static void nil_retain_native(object* obj, clockwork_vm* vm)
{
    vm_pushSelf(vm);
    vm_return(vm);
}

static void nil_release_native(object* obj, clockwork_vm* vm)
{
    vm_pushNil(vm);
    vm_return(vm);
}

#pragma mark - Native Methods

class* nil_class(struct clockwork_vm* vm)
{
    class* nilClass = class_init(vm, "Nil", "Object");

    {
        block* isNilMethodNative = block_init_native(vm, NULL, &nil_isNil_native);
        class_addClassMethod(nilClass, vm, "isNil", isNilMethodNative);
        class_addInstanceMethod(nilClass, vm, "isNil", isNilMethodNative);
    }

    {
        block* isTrueMethodNative = block_init_native(vm, NULL, &nil_isTrue_native);
        class_addClassMethod(nilClass, vm, "isTrue", isTrueMethodNative);
        class_addInstanceMethod(nilClass, vm, "isTrue", isTrueMethodNative);
    }

    {
        block* isFalseMethodNative = block_init_native(vm, NULL, &nil_isFalse_native);
        class_addClassMethod(nilClass, vm, "isFalse", isFalseMethodNative);
        class_addInstanceMethod(nilClass, vm, "isFalse", isFalseMethodNative);
    }

    {
        block* deallocMethodNative = block_init_native(vm, NULL, &nil_dealloc_native);
        class_addClassMethod(nilClass, vm, "dealloc", deallocMethodNative);
        class_addInstanceMethod(nilClass, vm, "dealloc", deallocMethodNative);
    }

    {
        class_addInstanceMethod(nilClass, vm, "retain", block_init_native(vm, NULL, &nil_retain_native));
        class_addInstanceMethod(nilClass, vm, "release", block_init_native(vm, NULL, &nil_release_native));
    }

#warning ADD forwardInvocation: TO DO NOTHING (IT IS SAFE TO MESSAGE nil/Nil).
#warning TODO: PUT IN EXCEPTION THROW ON INSTANCE CREATION

    return nilClass;
}

object* nil_instance(clockwork_vm* vm)
{
    object* nilSuper = object_init(vm);
    object* nilObj = object_create_super(vm, nilSuper, (class*)vm_getConstant(vm, "Nil"), sizeof(void*) * 3);
    return nilObj;
}

int object_isNil(object* obj, clockwork_vm* vm)
{
    return object_isKindOfClass_native(obj, (class*)vm_getConstant(vm, "Nil"));
}
