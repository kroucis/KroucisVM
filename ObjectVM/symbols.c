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
    struct object_header header;

#ifdef SYM_CACHE_HASH
    uint32_t hash;
#endif

    char* sym;
};

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
    clkwk_makeStringCstr(vm, symbol_cstr(sym));
    clkwk_return(vm);
}

class* symbol_class(clockwork_vm* vm)
{
    class* sym_class = class_init(vm, "Symbol", "Object");

    {
        class_addInstanceMethod(sym_class, vm, "toString", block_init_native(vm, 0, 0, &symbol_toString_native));
        class_addInstanceMethod(sym_class, vm, "description", block_init_native(vm, 0, 0, &symbol_toString_native));
    }

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
        sym = clkwk_allocate(vm, sizeof(symbol));
#ifdef SYM_CACHE_HASH
        sym->hash = hash;
#endif
        sym->sym = clkwk_allocate(vm, strlen(s) + 1);
        strcpy(sym->sym, s);

        if (table->count + 1 < table->capacity)
        {
            table->symbols[table->count++] = sym;
        }
        else
        {
#warning GROW SYMBOL TABLE
            printf("NEED TO EXPAND SYMBOL TABLE!");
        }
    }

    return sym;
}

char* symbol_cstr(symbol* sym)
{
    return sym->sym;
}
