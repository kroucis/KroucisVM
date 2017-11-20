
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
#include "array.h"
#include "symbols.h"

#include "clkwk_debug.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>

#pragma mark Object

struct object
{
    struct object_header header;
};

/*
 + alloc
 + forwardMessage:withArguments:
 + isNil
 + isTrue
 + isFalse
 + retain
 + release
 + addInstanceMethod:withNumArguments:atPC:
 + addClassMethod:withNumArguments:atPC:
 + description
 + class

 - init
 - dealloc
 - retain
 - release
 - isNil
 - isTrue
 - isFalse
 - hash
 - isEqual
 - respondsToSelector:
 - description
 - class
 */

#pragma mark - Bound Methods

static void class_addInstanceMethod_withImplBlock_native(object* klass, clockwork_vm* vm)
{
    symbol* name = (symbol*)clkwk_getLocal(vm, 0);
    block* blk = (block*)clkwk_getLocal(vm, 1);
    class_addInstanceMethod((class*)klass, vm, symbol_cstr(name), blk);
    clkwk_push(vm, klass);
    clkwk_return(vm);
}

static void class_addClassMethod_withImplBlock_native(object* klass, clockwork_vm* vm)
{
    symbol* name = (symbol*)clkwk_getLocal(vm, 0);
    block* blk = (block*)clkwk_getLocal(vm, 1);
    class_addClassMethod((class*)klass, vm, symbol_cstr(name), blk);
    clkwk_push(vm, klass);
    clkwk_return(vm);
}

static void class_release_native(object* klass, clockwork_vm* vm)
{
    clkwk_pushNil(vm);
    clkwk_return(vm);
}

static void class_retain_native(object* klass, clockwork_vm* vm)
{
    clkwk_pushSelf(vm);
    clkwk_return(vm);
}

static void object_init_native(object* instance, clockwork_vm* vm)
{
    if (instance->header.super)
    {
        clkwk_pushSuper(vm);
        clkwk_dispatch(vm, "init", 0);
        clkwk_pop(vm);
        clkwk_push(vm, instance);
    }
    else
    {
        instance->header.retainCount = 1;

        clkwk_push(vm, instance);
    }

    clkwk_return(vm);
}

static void object_dealloc_native(object* obj, clockwork_vm* vm)
{
    object_dealloc(obj, vm);

    clkwk_pushNil(vm);

    clkwk_return(vm);
}

static void object_retain_native(object* instance, clockwork_vm* vm)
{
    object_retain(instance, vm);

    clkwk_pushSelf(vm);

    clkwk_return(vm);
}

static void object_release_native(object* instance, clockwork_vm* vm)
{
    object_release(instance, vm);

    clkwk_pushNil(vm);

    clkwk_return(vm);
}

static void object_isNil_native(object* klass, clockwork_vm* vm)
{
    clkwk_pushFalse(vm);

    clkwk_return(vm);
}

static void object_isTrue_native(object* klass, clockwork_vm* vm)
{
    clkwk_pushTrue(vm);

    clkwk_return(vm);
}

static void object_isFalse_native(object* klass, clockwork_vm* vm)
{
    clkwk_pushFalse(vm);

    clkwk_return(vm);
}

static void object_description_native(object* instance, clockwork_vm* vm)
{
    char s[50];
    int chars = 0;
    if (instance->header.isa != (class*)instance)
    {
        chars = sprintf(s, "<%s@%#016llx>", class_name(instance->header.isa, vm), (uint64_t)instance);
    }
    else
    {
        chars = sprintf(s, "<%s (Class)@%#016llx>", class_name(instance->header.isa, vm), (uint64_t)instance);
    }

    s[chars] = '\0';
    clkwk_pushStringCstr(vm, s);
    clkwk_return(vm);
}

static void object_class_native(object* instance, clockwork_vm* vm)
{
    clkwk_push(vm, (object*)object_getClass(instance, vm));
    clkwk_return(vm);
}

static void object_forwardMessage_withArguments_native(object* klass, clockwork_vm* vm)
{
#warning THROW EXCEPTION
    symbol* msg = (symbol*)clkwk_getLocal(vm, 0);
    printf("Object of class %s does not respond to selector '%s'\n", class_name(object_getClass(klass, vm), vm), symbol_cstr(msg));

    // Wow. Rough. Not very testable or informative. Thanks a lot, past me :|
    exit(EXIT_FAILURE);

    clkwk_return(vm);
}

static void class_alloc_native(object* klass, clockwork_vm* vm)
{
    object* super = NULL;
    if (klass->header.super)
    {
        clkwk_pushSuper(vm);
        clkwk_dispatch(vm, "alloc", 0);
        super = clkwk_pop(vm);
        object* allocd = object_create_super(vm, super, (class*)klass, object_instanceSize());
        clkwk_push(vm, allocd);
        clkwk_return(vm);
    }
    else
    {
        object* obj = object_init(vm);
        clkwk_push(vm, obj);
        clkwk_return(vm);
    }
}

static void class_dealloc_native(object* klass, clockwork_vm* vm)
{
    class* klazz = (class*)klass;
    class_dealloc(klazz, vm);

    clkwk_pushNil(vm);
    clkwk_return(vm);
}

static void class_forwardMessage_withArguments_native(object* klass, clockwork_vm* vm)
{
    symbol* message = (symbol*)clkwk_getLocal(vm, 0);
    array* argsArray = (array*)clkwk_getLocal(vm, 1);
    char* msgBytes = symbol_cstr(message);

    CLKWK_ASSERT_NOT_NULL(message);
    CLKWK_ASSERT_NOT_NULL(argsArray);
    CLKWK_ASSERT_NOT_NULL(msgBytes);

    if (strncmp(msgBytes, "new", 3) != 0)
    {
        if (klass->header.super)
        {
            clkwk_pushSuper(vm);
            clkwk_push(vm, (object*)argsArray);
            clkwk_push(vm, (object*)message);
            clkwk_dispatch(vm, "forwardMessage:withArguments:", 2);
        }
        else
        {
#warning THROW EXCEPTION
            CLKWK_DBGPRNT("COULD NOT FIND HANLDER FOR FORWARDED MESSAGE %s. EXITING WITH FAILURE!\n", symbol_cstr(message));
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        clkwk_pushSelf(vm);

        clkwk_dispatch(vm, "alloc", 0);

        size_t len = strlen(msgBytes
                            );
        uint64_t argCount = 0;
        char initMessage[len + 2];
        initMessage[0] = 'i'; initMessage[1] = 'n'; initMessage[2] = 'i'; initMessage[3] = 't';
        if (len > 3)
        {
            strcpy(initMessage + 4, msgBytes + 3);
            argCount = array_count(argsArray, vm);
            for (uint64_t i = 0; i < argCount; i++)
            {
                clkwk_push(vm, array_objectAtIndex(argsArray, vm, i));
            }
        }

        initMessage[len + 1] = '\0';

        clkwk_dispatch(vm, initMessage, argCount);
    }

    clkwk_return(vm);
}

static void object_respondsToSelector_native(object* instance, clockwork_vm* vm)
{
    sel* selector = (sel*)clkwk_getLocal(vm, 0);
    if (object_respondsToSelector(instance, vm, clkwk_getSymbol(vm, selector)))
    {
        clkwk_pushTrue(vm);
    }
    else
    {
        clkwk_pushFalse(vm);
    }

    clkwk_return(vm);
}

static void object_hash_native(object* instance, clockwork_vm* vm)
{
    integer* hash = integer_init(vm, (int64_t)instance);
    clkwk_push(vm, (object*)hash);

    clkwk_return(vm);
}

static void object_isEqual_native(object* instance, clockwork_vm* vm)
{
    object* other = clkwk_getLocal(vm, 0);
    if (other == instance)  // Direct pointer compare
    {
        clkwk_pushTrue(vm);
    }
    else
    {
        clkwk_pushFalse(vm);
    }

    clkwk_return(vm);
}

#pragma mark - Native Methods

class* object_class(clockwork_vm* vm)
{
    class* objectClass = class_init(vm, "Object", NULL);

    // Class Methods
    {
        class_addClassMethod(objectClass, vm, "forwardMessage:withArguments:", block_init_native(vm, 2, 0, &class_forwardMessage_withArguments_native));

        class_addClassMethod(objectClass, vm, "alloc", block_init_native(vm, 0, 0, &class_alloc_native));

        class_addClassMethod(objectClass, vm, "dealloc", block_init_native(vm, 0, 0, &class_dealloc_native));

        class_addClassMethod(objectClass, vm, "retain", block_init_native(vm, 0, 0, &class_retain_native));
        class_addClassMethod(objectClass, vm, "release", block_init_native(vm, 0, 0, &class_release_native));

        class_addClassMethod(objectClass, vm, "addInstanceMethod:withImplBlock:", block_init_native(vm, 2, 0, &class_addInstanceMethod_withImplBlock_native));

        class_addClassMethod(objectClass, vm, "addClassMethod:withImplBlock:", block_init_native(vm, 2, 0, &class_addClassMethod_withImplBlock_native));
    }

    // Instance Methods
    {
        class_addInstanceMethod(objectClass, vm, "init", block_init_native(vm, 0, 0, &object_init_native));

        class_addInstanceMethod(objectClass, vm, "dealloc", block_init_native(vm, 0, 0, &object_dealloc_native));

        class_addInstanceMethod(objectClass, vm, "retain", block_init_native(vm, 0, 0, &object_retain_native));
        class_addInstanceMethod(objectClass, vm, "release", block_init_native(vm, 0, 0, &object_release_native));

        class_addInstanceMethod(objectClass, vm, "respondsToSelector:", block_init_native(vm, 1, 0, &object_respondsToSelector_native));

        class_addInstanceMethod(objectClass, vm, "hash", block_init_native(vm, 0, 0, &object_hash_native));

        class_addInstanceMethod(objectClass, vm, "forwardMessage:withArguments:", block_init_native(vm, 2, 0, &object_forwardMessage_withArguments_native));
    }

    // BOTH Methods
    {
        block* isNilMethodNative = block_init_native(vm, 0, 0, &object_isNil_native);
        class_addClassMethod(objectClass, vm, "isNil", isNilMethodNative);
        class_addInstanceMethod(objectClass, vm, "isNil", isNilMethodNative);

        block* isTrueMethodNative = block_init_native(vm, 0, 0, &object_isTrue_native);
        class_addClassMethod(objectClass, vm, "isTrue", isTrueMethodNative);
        class_addInstanceMethod(objectClass, vm, "isTrue", isTrueMethodNative);

        block* isFalseMethodNative = block_init_native(vm, 0, 0, &object_isFalse_native);
        class_addClassMethod(objectClass, vm, "isFalse", isFalseMethodNative);
        class_addInstanceMethod(objectClass, vm, "isFalse", isFalseMethodNative);

        block* descriptionMethod = block_init_native(vm, 0, 0, &object_description_native);
        class_addClassMethod(objectClass, vm, "description", descriptionMethod);
        class_addInstanceMethod(objectClass, vm, "description", descriptionMethod);

        block* isEqualMethod = block_init_native(vm, 1, 0, &object_isEqual_native);
        class_addInstanceMethod(objectClass, vm, "isEqual:", isEqualMethod);
        class_addClassMethod(objectClass, vm, "isEqual:", isEqualMethod);

        block* classMethod = block_init_native(vm, 0, 0, &object_class_native);
        class_addInstanceMethod(objectClass, vm, "class", classMethod);
        class_addClassMethod(objectClass, vm, "class", classMethod);
    }

#ifdef CLKWK_PRINT_SIZES
    CLKWK_DBGPRNT("Object: %lu\n", sizeof(object));
#endif

#warning -isKindOfClass:
#warning -isMemberOfClass:

    return objectClass;
}

uint32_t object_instanceSize(void)
{
    return sizeof(object);
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
        return object_isKindOfClass_native(obj->header.super, klass);
    }
}

boolean object_isMemberOfClass_native(object* obj, class* klass)
{
    return obj->header.isa == klass;
}

object* object_init(clockwork_vm* vm)
{
    object* obj = clkwk_allocate(vm, sizeof(object));
    obj->header.retainCount = 1;
    obj->header.isa = (class*)clkwk_getConstant(vm, "Object");
    obj->header.super = NULL;
    obj->header.size = sizeof(object);
    obj->header.extra = NULL;
    return obj;
}

object* object_create_super(struct clockwork_vm* vm, object* sup, struct class* klass, uint32_t bytes)
{
    object* obj = clkwk_allocate(vm, bytes);
    obj->header.isa = klass;
    obj->header.super = sup;
    obj->header.size = bytes;
    obj->header.extra = NULL;
    return obj;
}

uint32_t object_size(object* instance)
{
    return instance->header.size;
}

void object_dealloc(object* instance, clockwork_vm* vm)
{
    char s[50];
    int chars = sprintf(s, "<%s@%#016llx>", class_name(instance->header.isa, vm), (uint64_t)instance);
    s[chars] = '\0';
    CLKWK_DBGPRNT("Deallocating instance %s\n", s);
    if (instance->header.ivars)
    {
        primitive_table_dealloc(instance->header.ivars, vm, Yes);
    }
    clkwk_free(vm, instance);
}

void object_setIvar(object* instance, clockwork_vm* vm, symbol* ivar, object* value)
{
    if (instance->header.ivars == NULL)
    {
        instance->header.ivars = primitive_table_init(vm, 5);
    }
#warning GROW ivar TABLE WHEN NEEDED

    object* old = primitive_table_get(instance->header.ivars, vm, symbol_cstr(ivar));
    if (value != old)
    {
        primitive_table_set(instance->header.ivars, vm, symbol_cstr(ivar), value);
        if (old)
        {
            object_release(old, vm);
        }
    }

#warning TODO: LOOK IN SUPER FOR ivars IF WE CANNOT FIND THEM HERE.
}

object* object_getIvar(object* instance, clockwork_vm* vm, symbol* ivar)
{
    assert(instance);
    assert(vm);
    assert(ivar);

    object* value = NULL;
    if (instance->header.ivars != NULL)
    {
        value = primitive_table_get(instance->header.ivars, vm, symbol_cstr(ivar));
    }

#warning TODO: LOOK IN SUPER FOR ivars IF WE CANNOT FIND THEM HERE.

    return value;
}

object* object_retain(object* instance, clockwork_vm* vm)
{
    assert(instance);
    assert(vm);

    instance->header.retainCount++;

    return instance;
}

void object_release(object* instance, clockwork_vm* vm)
{
    assert(instance);
    assert(vm);

    instance->header.retainCount--;

    if (instance->header.retainCount == 0)
    {
        clkwk_push(vm, instance);
        clkwk_dispatch(vm, "dealloc", 0);
        clkwk_pop(vm);
    }
    else if (instance->header.retainCount < 0)
    {
        printf("OBJECT 0x%lld WAS UNDER-RETAINED!\n", (uint64_t)instance);
    }
}

boolean object_respondsToSelector(object* instance, clockwork_vm* vm, symbol* selector)
{
    int yn = No;
    if (instance)
    {
        if ((object*)instance->header.isa != instance)
        {
            yn = class_getInstanceMethod(instance->header.isa, vm, selector) != NULL;
            if (!yn)
            {
                yn = object_respondsToSelector(instance->header.super, vm, selector);
            }
        }
        else
        {
            yn = class_getInstanceMethod(instance->header.isa, vm, selector) != NULL;
            if (!yn)
            {
                yn = class_getClassMethod(instance->header.isa, vm, selector) != NULL;
                if (!yn)
                {
                    yn = object_respondsToSelector(instance->header.super, vm, selector);
                }
            }
        }
    }

    return yn;
}

block* object_findInstanceMethod(object* instance, clockwork_vm* vm, symbol* selector)
{

    block* m = NULL;
    if (instance)
    {
        m = class_getInstanceMethod(instance->header.isa, vm, selector);
        if (!m)
        {
            m = object_findInstanceMethod(instance->header.super, vm, selector);
        }
    }

    return m;
}

block* object_findMethod(object* instance, clockwork_vm* vm, symbol* selector)
{
    if (instance != (object*)instance->header.isa)
    {
        return object_findInstanceMethod(instance, vm, selector);
    }
    else
    {
        return class_findClassMethod((class *)instance, vm, selector);
    }
}

void object_setSuper(object* instance, clockwork_vm* vm, object* sup)
{
    instance->header.super = sup;
}

object* object_super(object* instance, clockwork_vm* vm)
{
    return instance->header.super;
}

class* object_getClass(object* instance, clockwork_vm* vm)
{
    return instance->header.isa;
}
