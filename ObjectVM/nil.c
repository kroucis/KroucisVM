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
#include "symbols.h"

#include <stdlib.h>
#include <stdio.h>

#pragma mark Nil

#pragma mark - Bound Methods

static void nil_isNil_native(object* obj, clockwork_vm* vm)
{
    clkwk_pushTrue(vm);
    clkwk_return(vm);
}

static void nil_isTrue_native(object* obj, clockwork_vm* vm)
{
    clkwk_pushFalse(vm);
    clkwk_return(vm);
}

static void nil_isFalse_native(object* obj, clockwork_vm* vm)
{
    clkwk_pushTrue(vm);
    clkwk_return(vm);
}

static void nil_dealloc_native(object* obj, clockwork_vm* vm)
{
    clkwk_pushNil(vm);
    clkwk_return(vm);
}

static void nil_retain_native(object* obj, clockwork_vm* vm)
{
    clkwk_pushSelf(vm);
    clkwk_return(vm);
}

static void nil_release_native(object* obj, clockwork_vm* vm)
{
    clkwk_pushNil(vm);
    clkwk_return(vm);
}

static void nil_forwardMessage_withArguments_native(object* obj, clockwork_vm* vm)
{
    symbol* msg = (symbol*)clkwk_getLocal(vm, 0);

    printf("DEBUG: nil was sent %s. Ignoring.\n", symbol_cstr(msg));
    clkwk_pushNil(vm);
    clkwk_return(vm);
}

static void nil_description_native(object* obj, clockwork_vm* vm)
{
    clkwk_makeStringCstr(vm, "nil");
    clkwk_return(vm);
}

#pragma mark - Native Methods

class* nil_class(struct clockwork_vm* vm)
{
    class* nilClass = class_init(vm, "Nil", "Object");

    {
        block* isNilMethodNative = block_init_native(vm, 0, 0, &nil_isNil_native);
        class_addClassMethod(nilClass, vm, "isNil", isNilMethodNative);
        class_addInstanceMethod(nilClass, vm, "isNil", isNilMethodNative);
    }

    {
        block* isTrueMethodNative = block_init_native(vm, 0, 0, &nil_isTrue_native);
        class_addClassMethod(nilClass, vm, "isTrue", isTrueMethodNative);
        class_addInstanceMethod(nilClass, vm, "isTrue", isTrueMethodNative);
    }

    {
        block* isFalseMethodNative = block_init_native(vm, 0, 0, &nil_isFalse_native);
        class_addClassMethod(nilClass, vm, "isFalse", isFalseMethodNative);
        class_addInstanceMethod(nilClass, vm, "isFalse", isFalseMethodNative);
    }

    {
        block* deallocMethodNative = block_init_native(vm, 0, 0, &nil_dealloc_native);
        class_addClassMethod(nilClass, vm, "dealloc", deallocMethodNative);
        class_addInstanceMethod(nilClass, vm, "dealloc", deallocMethodNative);
    }

    {
        class_addInstanceMethod(nilClass, vm, "retain", block_init_native(vm, 0, 0, &nil_retain_native));
        class_addInstanceMethod(nilClass, vm, "release", block_init_native(vm, 0, 0, &nil_release_native));
    }

    {
        block* forwardMethodNative = block_init_native(vm, 2, 0, &nil_forwardMessage_withArguments_native);
        class_addInstanceMethod(nilClass, vm, "forwardMessage:withArguments:", forwardMethodNative);
        class_addClassMethod(nilClass, vm, "forwardMessage:withArguments:", forwardMethodNative);
    }

    {
        class_addInstanceMethod(nilClass, vm, "description", block_init_native(vm, 0, 0, &nil_description_native));
    }

#warning TODO: PUT IN EXCEPTION THROW ON INSTANCE CREATION

    return nilClass;
}

object* nil_instance(clockwork_vm* vm)
{
    object* nilSuper = object_init(vm);
    object* nilObj = object_create_super(vm, nilSuper, (class*)clkwk_getConstant(vm, "Nil"), sizeof(struct object_header));
    return nilObj;
}

int object_isNil(object* obj, clockwork_vm* vm)
{
    return object_isKindOfClass_native(obj, (class*)clkwk_getConstant(vm, "Nil"));
}
