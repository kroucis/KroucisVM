//
//  str.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-15.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "str.h"

#include "class.h"
#include "object.h"
#include "vm.h"
#include "block.h"
#include "primitive_table.h"
#include "integer.h"

#include <stdlib.h>
#include <string.h>

struct str
{
    class* isa;
    object* super;
    primitive_table* ivars;

    uint32_t length;
    char* data;
};

static void string_init_native(object* klass, clockwork_vm* vm)
{

}

static void string_length_native(object* instance, clockwork_vm* vm)
{
    str* s = (str*)instance;
    integer* len = integer_init(vm, str_length(s, vm));
    vm_push(vm, (object*)len);
}

static void string_dealloc_native(object* instance, clockwork_vm* vm)
{
    str_dealloc((str*)instance, vm);
    vm_pushNil(vm);
}

struct class* string_class(clockwork_vm* vm)
{
    class* stringClass = class_init(vm, "String", "Object");

    class_addInstanceMethod(stringClass, vm, "length", block_init_native(vm, NULL, &string_length_native));

    class_addInstanceMethod(stringClass, vm, "dealloc", block_init_native(vm, NULL, &string_dealloc_native));

    return stringClass;
}

str* str_init(clockwork_vm* vm, const char* const data)
{
    return str_init_len(vm, data, (uint32_t)strlen(data));
}

str* str_init_len(clockwork_vm* vm, const char* const data, uint32_t len)
{
    object* strSuper = object_init(vm);
    str* string = (str*)object_create_super(vm, strSuper, (class*)vm_getConstant(vm, "String"), sizeof(str));
    if (len > 0)
    {
        string->data = vm_allocate(vm, len);
        memcpy(string->data, (void*)data, len);
    }
    string->length = len;
    return string;
}

void str_dealloc(str* string, clockwork_vm* vm)
{
    if (str_length(string, vm) > 0)
    {
        vm_free(vm, string->data);
    }
    vm_free(vm, string);
}

uint32_t str_length(str* string, clockwork_vm* vm)
{
    return string->length;
}

char* str_raw_bytes(str* string, clockwork_vm* vm)
{
    return string->data;
}

void str_into_cstr(str* string, struct clockwork_vm* vm, char* outCStr)
{
    memcpy(outCStr, string->data, string->length);
    outCStr[string->length + 1] = '\0';
}

int str_compare(str* s0, clockwork_vm* vm, str* s1)
{
    size_t min_len = s0->length < s1->length ? s0->length : s1->length;
    return strncmp(s0->data, s1->data, min_len);
}
