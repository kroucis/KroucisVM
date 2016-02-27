//
//  symbols.c
//  ObjectVM
//
//  Created by Kyle Roucis on 14-4-4.
//  Copyright (c) 2014 Kyle Roucis. All rights reserved.
//

#include "symbols.h"

#include "vm.h"
#include "object.h"
#include "class.h"
#include "block.h"

#include <string.h>
#include <stdio.h>

#define SYM_CACHE_HASH

struct symbol
{
    struct object_header header;        /* 40 */
    char* sym;                          /* 8 */

#ifdef SYM_CACHE_HASH
    uint32_t hash;                      /* 4 */
    char _padding[4];                   /* 4 */
#endif
                                    /* = 48 | 56  */
};

/*
 - toString
 - description
 */

struct symbol_table
{
    uint64_t count;
    uint64_t capacity;
    symbol** symbols;
};

#pragma mark - Private Helpers

static uint32_t sym_hash(const char* s)
{
    uint32_t hashval = 0;
    int i = 0;

    /* Convert our string to an integer */
    while (hashval < UINT32_MAX && i < strlen(s)) {
        hashval = hashval << 8;
        hashval += s[i];
        i++;
    }

    return hashval;
}

#pragma mark - Bound Methods

static void symbol_toString_native(object* slf, clockwork_vm* vm)
{
    symbol* sym = (symbol*)slf;
    clkwk_pushStringCstr(vm, symbol_cstr(sym));
    clkwk_return(vm);
}

static void symbol_description_native(object* slf, clockwork_vm* vm)
{
    symbol* sym = (symbol*)slf;
    char desc[strlen(sym->sym) + 1];
    sprintf(desc, ":%s", sym->sym);
    clkwk_pushStringCstr(vm, desc);
    clkwk_return(vm);
}

#pragma mark - Private Methods

//static symbol* symbol_init(const char* s, clockwork_vm* vm)
//{
//    object* symSuper = object_init(vm);
//    symbol* sym = (symbol*)object_create_super(vm, symSuper, (class*)clkwk_getConstant(vm, "Symbol"), sizeof(symbol));
//    sym->sym = clkwk_allocate(vm, strlen(s) + 1);
//    strcpy(sym->sym, s);
//#ifdef SYM_CACHE_HASH
//    sym->hash = sym_hash(s);
//#endif
//
//    return sym;
//}

static symbol* symbol_initWithHash(const char* s, uint32_t hash, clockwork_vm* vm)
{
    object* symSuper = object_init(vm);
    symbol* sym = (symbol*)object_create_super(vm, symSuper, (class*)clkwk_getConstant(vm, "Symbol"), sizeof(symbol));
    sym->sym = clkwk_allocate(vm, strlen(s) + 1);
    strcpy(sym->sym, s);
#ifdef SYM_CACHE_HASH
    sym->hash = hash;
#endif

    return sym;
}

class* symbol_class(clockwork_vm* vm)
{
    class* sym_class = class_init(vm, "Symbol", "Object");

    // Instance Methods
    {
        class_addInstanceMethod(sym_class, vm, "toString", block_init_native(vm, 0, 0, &symbol_toString_native));
        class_addInstanceMethod(sym_class, vm, "description", block_init_native(vm, 0, 0, &symbol_description_native));
    }

#ifdef CLKWK_PRINT_SIZES
    CLKWK_DBGPRNT("Symbol: %lu\n", sizeof(symbol));
#endif

    return sym_class;
}

symbol_table* symbol_table_init(clockwork_vm* vm)
{
    symbol_table* st = clkwk_allocate(vm, sizeof(symbol_table));
    st->count = 0;
    st->symbols = clkwk_allocate(vm, sizeof(symbol) * 16);
    st->capacity = 16;

    return st;
}

void symbol_table_dealloc(symbol_table* table, struct clockwork_vm* vm)
{
    for (int i = 0; i < table->count; i++)
	{
		clkwk_free(vm, table->symbols[i]->sym);
        clkwk_free(vm, table->symbols[i]);
	}

    clkwk_free(vm, table->symbols);
    clkwk_free(vm, table);
}

symbol* symbol_table_get(symbol_table* table, const char* s, clockwork_vm* vm)
{
    uint32_t hash = sym_hash(s);
    symbol* sym = NULL;
    for (int i = 0; i < table->count; i++)
	{
        symbol* cur = table->symbols[i];
#ifdef SYM_CACHE_HASH
		if (cur->hash == hash && (strcmp(s, cur->sym) == 0))
        {
            sym = cur;
            break;
        }
#else
        uint32_t cur_hash = sym_hash(cur->sym);
        if (cur_hash == hash && (strcmp(s, cur->sym) == 0))
        {
            sym = cur;
            break;
        }
#endif
	}

    if (!sym)
    {
        sym = symbol_initWithHash(s, hash, vm);

        if (table->count + 1 < table->capacity)
        {
            table->symbols[table->count++] = sym;
        }
        else
        {
            uint64_t oldCap = table->capacity;
            table->capacity *= 2;
            symbol** newTable = clkwk_allocate(vm, sizeof(symbol) * table->capacity);
            memcpy(newTable, table->symbols, sizeof(symbol) * oldCap);
            clkwk_free(vm, table->symbols);
            table->symbols = newTable;

            table->symbols[table->count++] = sym;
        }
    }

    return sym;
}

char* symbol_cstr(symbol* sym)
{
    return sym->sym;
}
