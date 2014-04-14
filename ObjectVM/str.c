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

#include "clkwk_debug.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#pragma mark String

struct str
{
    struct object_header header;    /* 40 */

    uint32_t length;                /* 8 */
    char* data;                     /* 8 */
                                 /* = 56 */
};

#pragma mark - Bound Methods

static void string_alloc_native(object* klass, clockwork_vm* vm)
{
    clkwk_push(vm, (object*)str_init(vm, NULL));
    clkwk_return(vm);
}

static void string_length_native(object* instance, clockwork_vm* vm)
{
    str* s = (str*)instance;
    integer* len = integer_init(vm, str_length(s, vm));
    clkwk_push(vm, (object*)len);
    clkwk_return(vm);
}

static void string_dealloc_native(object* instance, clockwork_vm* vm)
{
    str* string = (str*)instance;
    if (str_length(string, vm) > 0)
    {
        clkwk_free(vm, string->data);
    }
    clkwk_pushSuper(vm);
    clkwk_dispatch(vm, "dealloc", 0);
    clkwk_free(vm, instance);
    clkwk_pop(vm);
    clkwk_pushNil(vm);
    clkwk_return(vm);
}

static void string_hash_native(object* instance, clockwork_vm* vm)
{
    str* string = (str*)instance;
    int64_t hash = str_hash(string, vm);
    integer* i_hash = integer_init(vm, hash);
    clkwk_push(vm, (object*)i_hash);
    clkwk_return(vm);
}

static void string_print_native(object* instance, clockwork_vm* vm)
{
    str* string = (str*)instance;
    printf("%.*s", str_length(string, vm), str_raw_bytes(string, vm));
    clkwk_pushNil(vm);
    clkwk_return(vm);
}

static void string_puts_native(object* instance, clockwork_vm* vm)
{
    str* string = (str*)instance;
    printf("%.*s\n", str_length(string, vm), str_raw_bytes(string, vm));
    clkwk_pushNil(vm);
    clkwk_return(vm);
}

#pragma mark - Native Methods

struct class* string_class(clockwork_vm* vm)
{
    class* stringClass = class_init(vm, "String", "Object");

    class_addClassMethod(stringClass, vm, "alloc", block_init_native(vm, 0, 0, &string_alloc_native));
    class_addInstanceMethod(stringClass, vm, "length", block_init_native(vm, 0, 0, &string_length_native));
    class_addInstanceMethod(stringClass, vm, "dealloc", block_init_native(vm, 0, 0, &string_dealloc_native));
    class_addInstanceMethod(stringClass, vm, "hash", block_init_native(vm, 0, 0, &string_hash_native));
    class_addInstanceMethod(stringClass, vm, "print", block_init_native(vm, 0, 0, &string_print_native));
    class_addInstanceMethod(stringClass, vm, "puts", block_init_native(vm, 0, 0, &string_puts_native));

#ifdef CLKWK_PRINT_SIZES
    CLKWK_DBGPRNT("String: %lu\n", sizeof(str));
#endif

    return stringClass;
}

str* str_init(clockwork_vm* vm, const char* const data)
{
    assert(vm);
    assert(data);

    uint32_t len = data ? (uint32_t)strlen(data) : 0;
    return str_init_len(vm, data, len);
}

str* str_init_len(clockwork_vm* vm, const char* const data, uint32_t len)
{
    assert(vm);
    assert(data);

    object* strSuper = object_init(vm);
    str* string = (str*)object_create_super(vm, strSuper, (class*)clkwk_getConstant(vm, "String"), sizeof(str));
    if (len > 0)
    {
        string->data = clkwk_allocate(vm, len);
        memcpy(string->data, (void*)data, len);
    }
    string->length = len;
    string->header.size = sizeof(str);
    
    return string;
}

void str_dealloc(str* string, clockwork_vm* vm)
{
    assert(string);
    assert(vm);

    if (str_length(string, vm) > 0)
    {
        clkwk_free(vm, string->data);
    }
    clkwk_free(vm, (object*)string);
}

uint32_t str_length(str* string, clockwork_vm* vm)
{
    assert(string);
    assert(vm);

    return string->length;
}

char* str_raw_bytes(str* string, clockwork_vm* vm)
{
    assert(string);
    assert(vm);

    return string->data;
}

void str_into_cstr(str* string, clockwork_vm* vm, char* outCStr)
{
    assert(string);
    assert(vm);
    assert(outCStr);

    if (string->data != NULL)
    {
        memcpy(outCStr, string->data, string->length);
        outCStr[string->length + 1] = '\0';
    }
    else
    {
        outCStr[0] = '\0';
    }
}

int str_compare(str* s0, clockwork_vm* vm, str* s1)
{
    assert(s0);
    assert(vm);
    assert(s1);

    size_t min_len = s0->length < s1->length ? s0->length : s1->length;
    return strncmp(s0->data, s1->data, min_len);
}

int64_t str_hash(str* string, clockwork_vm* vm)
{
    assert(string);
    assert(vm);
    
    int64_t hashval = 0;

    int len_plus_one = str_length(string, vm) + 1;
    char temp[len_plus_one];
    str_into_cstr(string, vm, (char*)&temp);

	int i = 0;
	while (hashval < INT64_MAX && i < strlen(temp)) {
		hashval = hashval << 8;
		hashval += temp[i];
		i++;
	}

	return hashval;
}
