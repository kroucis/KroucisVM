//
//  object.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-10.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "object.h"

#include "vm.h"
#include "primitive_table.h"
#include "block.h"
#include "class.h"
#include "str.h"
#include "integer.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

struct object
{
    class* isa;
    struct object* super;
    primitive_table* ivars;

    int32_t retainCount;
};

static void class_addInstanceMethod_native(object* klass, clockwork_vm* vm)
{
    str* name = (str*)vm_getLocal(vm, "selector");
    block* impl = (block*)vm_getLocal(vm, "impl");
    class_addInstanceMethod((class*)klass, vm, str_raw_bytes(name, vm), impl);
    vm_push(vm, klass);
}

static void class_addClassMethod_native(object* klass, clockwork_vm* vm)
{
    str* name = (str*)vm_getLocal(vm, "selector");
    block* impl = (block*)vm_getLocal(vm, "impl");
    class_addClassMethod((class*)klass, vm, str_raw_bytes(name, vm), impl);
    vm_push(vm, klass);
}

static void object_init_native(object* klass, clockwork_vm* vm)
{
    object* obj = vm_allocate(vm, sizeof(object));
    obj->isa = (class*)klass;
    obj->retainCount = 1;
    obj->ivars = NULL;
    obj->super = NULL;

    vm_push(vm, obj);
}

static void object_dealloc_native(object* obj, clockwork_vm* vm)
{
#warning ITERATE OVER ALL IVARS AND RELEASE THEM (IF NOT WEAK).
    printf("Deallocating instance 0x%lld\n", (uint64_t)obj);
    if (obj->ivars)
    {
        primitive_table_dealloc(obj->ivars, vm);
    }
    vm_free(vm, obj);

    vm_pushNil(vm);
}

static void object_retain_native(object* instance, clockwork_vm* vm)
{
    object_retain(instance, vm);

    vm_pushSelf(vm);
}

static void object_release_native(object* instance, clockwork_vm* vm)
{
    object_release(instance, vm);

    vm_pushNil(vm);
}

static void object_isNil_native(object* klass, clockwork_vm* vm)
{
    vm_pushFalse(vm);
}

static void object_isTrue_native(object* klass, clockwork_vm* vm)
{
    vm_pushTrue(vm);
}

static void object_isFalse_native(object* klass, clockwork_vm* vm)
{
    vm_pushFalse(vm);
}

static void object_forwardMessage_withArguments_native(object*klass, clockwork_vm* vm)
{
#warning THROW EXCEPTION
    signal(SIGKILL, NULL);
}

static void object_respondsToSelector_native(object* instance, clockwork_vm* vm)
{
    sel* selector = (sel*)vm_getLocal(vm, "selector");
    if (object_respondsToSelector(instance, vm, str_raw_bytes(selector, vm)))
    {
        vm_pushTrue(vm);
    }
    else
    {
        vm_pushFalse(vm);
    }
}

static void object_hash_native(object* instance, clockwork_vm* vm)
{
    integer* hash = integer_init(vm, (int64_t)instance);
    vm_push(vm, (object*)hash);
}

class* object_class(clockwork_vm* vm)
{
    class* objectClass = class_init(vm, "Object", NULL);

    {
        block* initMethodNative = block_init_native(vm, NULL, &object_init_native);
        class_addInstanceMethod(objectClass, vm, "init", initMethodNative);
    }

    {
        block* deallocMethodNative = block_init_native(vm, NULL, &object_dealloc_native);
        class_addInstanceMethod(objectClass, vm, "dealloc", deallocMethodNative);
    }

    {
        block* isNilMethodNative = block_init_native(vm, NULL, &object_isNil_native);
        class_addClassMethod(objectClass, vm, "isNil", isNilMethodNative);
        class_addInstanceMethod(objectClass, vm, "isNil", isNilMethodNative);
    }

    {
        block* isTrueMethodNative = block_init_native(vm, NULL, &object_isTrue_native);
        class_addClassMethod(objectClass, vm, "isTrue", isTrueMethodNative);
        class_addInstanceMethod(objectClass, vm, "isTrue", isTrueMethodNative);
    }

    {
        block* isFalseMethodNative = block_init_native(vm, NULL, &object_isFalse_native);
        class_addClassMethod(objectClass, vm, "isFalse", isFalseMethodNative);
        class_addInstanceMethod(objectClass, vm, "isFalse", isFalseMethodNative);
    }

    {
        block* retainMethod = block_init_native(vm, NULL, &object_retain_native);
        class_addInstanceMethod(objectClass, vm, "retain", retainMethod);
    }

    {
        block* releaseMethod = block_init_native(vm, NULL, &object_release_native);
        class_addInstanceMethod(objectClass, vm, "release", releaseMethod);
    }

    {
        local_scope* rts_ls = local_scope_init(vm);
        local_scope_addLocal(rts_ls, vm, "selector");
        block* respondsMethod = block_init_native(vm, rts_ls, &object_respondsToSelector_native);
        class_addInstanceMethod(objectClass, vm, "respondsToSelector:", respondsMethod);
    }

    {
        block* hashMethod = block_init_native(vm, NULL, &object_hash_native);
        class_addInstanceMethod(objectClass, vm, "hash", hashMethod);
    }

    {
        local_scope* aim_ls = local_scope_init(vm);
        local_scope_addLocal(aim_ls, vm, "selector");
        local_scope_addLocal(aim_ls, vm, "impl");
        block* addInstanceMethod = block_init_native(vm, aim_ls, &class_addInstanceMethod_native);
        class_addClassMethod(objectClass, vm, "addInstanceMethod:withImpl:", addInstanceMethod);
    }

    {
        local_scope* acm_ls = local_scope_init(vm);
        local_scope_addLocal(acm_ls, vm, "selector");
        local_scope_addLocal(acm_ls, vm, "impl");
        block* addClassMethod = block_init_native(vm, acm_ls, &class_addClassMethod_native);
        class_addClassMethod(objectClass, vm, "addClassMethod:withImpl:", addClassMethod);
    }

    {
        local_scope* fm_ls = local_scope_init(vm);
        local_scope_addLocal(fm_ls, vm, "message");
        local_scope_addLocal(fm_ls, vm, "args");
        block* forwardMessageMethod = block_init_native(vm, fm_ls, &object_forwardMessage_withArguments_native);
        class_addInstanceMethod(objectClass, vm, "forwardMessage:withArguments:", forwardMessageMethod);
        class_addClassMethod(objectClass, vm, "forwardMessage:withArguments:", forwardMessageMethod);
    }

#warning -isKindOfClass:
#warning -isMemberOfClass:

    return objectClass;
}

boolean object_isKindOfClass_native(object* obj, class* klass)
{
    if (obj == NULL)
    {
        return No;
    }
    else if (object_isMemberOfClass_native(obj, klass))
    {
        return Yes;
    }
    else
    {
        return object_isKindOfClass_native(obj->super, klass);
    }
}

boolean object_isMemberOfClass_native(object* obj, class* klass)
{
    return obj->isa == klass;
}

object* object_init(clockwork_vm* vm)
{
    object* obj = vm_allocate(vm, sizeof(object));
    obj->retainCount = 1;
    obj->isa = (class*)vm_getConstant(vm, "Object");
    obj->super = NULL;

    return obj;
}

object* object_create_super(struct clockwork_vm* vm, object* sup, struct class* klass, unsigned long bytes)
{
    object* obj = vm_allocate(vm, bytes);
    obj->isa = klass;
    obj->super = sup;
    return obj;
}

void object_dealloc(object* instance, clockwork_vm* vm)
{
    printf("Deallocating instance 0x%lld\n", (uint64_t)instance);
    if (instance->ivars)
    {
        primitive_table_dealloc(instance->ivars, vm);
    }
    vm_free(vm, instance);
}

void object_setIvar(object* instance, clockwork_vm* vm, char* ivar, object* value)
{
    if (instance->ivars == NULL)
    {
        instance->ivars = primitive_table_init(vm, 5);
    }
#warning GROW ivar TABLE WHEN NEEDED

    object* old = primitive_table_get(instance->ivars, vm, ivar);
    if (value != old)
    {
        primitive_table_set(instance->ivars, vm, ivar, value);
        if (old)
        {
            object_release(old, vm);
        }
    }

#warning TODO: LOOK IN SUPER FOR ivars IF WE CANNOT FIND THEM HERE.
}

object* object_getIvar(object* instance, clockwork_vm* vm, char* ivar)
{
    object* value = NULL;
    if (instance->ivars != NULL)
    {
        value = primitive_table_get(instance->ivars, vm, ivar);
    }

#warning TODO: LOOK IN SUPER FOR ivars IF WE CANNOT FIND THEM HERE.

    return value;
}

object* object_retain(object* instance, clockwork_vm* vm)
{
    instance->retainCount++;

    return instance;
}

void object_release(object* instance, clockwork_vm* vm)
{
    instance->retainCount--;

    if (instance->retainCount == 0)
    {
        vm_push(vm, instance);
        vm_dispatch(vm, "dealloc", 0);
        vm_pop(vm);
    }
    else if (instance->retainCount < 0)
    {
        printf("OBJECT 0x%lld WAS UNDER-RETAINED!\n", (uint64_t)instance);
    }
}

boolean object_respondsToSelector(object* instance, clockwork_vm* vm, char* selector)
{
    int yn = No;
    if (instance)
    {
        if ((object*)instance->isa != instance)
        {
            yn = class_getInstanceMethod(instance->isa, vm, selector) != NULL;
            if (!yn)
            {
                yn = object_respondsToSelector(instance->super, vm, selector);
            }
        }
        else
        {
            yn = class_getInstanceMethod(instance->isa, vm, selector) != NULL;
            if (!yn)
            {
                yn = class_getClassMethod(instance->isa, vm, selector) != NULL;
                if (!yn)
                {
                    yn = object_respondsToSelector(instance->super, vm, selector);
                }
            }
        }
    }

    return yn;
}

block* object_findInstanceMethod(object* instance, clockwork_vm* vm, char* selector)
{
    block* m = NULL;
    if (instance)
    {
        m = class_getInstanceMethod(instance->isa, vm, selector);
        if (!m)
        {
            m = object_findInstanceMethod(instance->super, vm, selector);
        }
    }

    return m;
}

block* object_findMethod(object* instance, clockwork_vm* vm, char* selector)
{
    if (instance != (object*)instance->isa)
    {
        return class_findClassMethod((class *)instance, vm, selector);
    }
    else
    {
        return object_findInstanceMethod(instance, vm, selector);
    }
}

void object_setSuper(object* instance, struct clockwork_vm* vm, object* sup)
{
    instance->super = sup;
}
