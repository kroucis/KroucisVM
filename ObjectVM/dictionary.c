//
//  object_table.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-15.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "dictionary.h"

#include "object.h"
#include "vm.h"
#include "str.h"
#include "class.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

struct dictionary_entry
{
    str* key;
    object* value;
    struct dictionary_entry* next;
};

struct dictionary
{
    uint32_t capacity;
    uint32_t count;
    struct dictionary_entry** entries;
};

static uint32_t _hash_string(str* string, clockwork_vm* vm)
{
    unsigned int hashval = 0;
	int i = 0;

	/* Convert our string to an integer */
	while (hashval < UINT_MAX && i < str_length(string, vm)) {
		hashval = hashval << 8;
		hashval += str_raw_bytes(string, vm)[i];
		i++;
	}

	return hashval;
}

static struct dictionary_entry* _new_entry(clockwork_vm* vm, str* key, object* value)
{
    struct dictionary_entry* entry = clkwk_allocate(vm, sizeof(struct dictionary_entry));
    entry->key = str_init_len(vm, str_raw_bytes(key, vm), str_length(key, vm));
    entry->value = object_retain(value, vm);
    entry->next = NULL;
    return entry;
}

dictionary* dictionary_init(clockwork_vm* vm, unsigned int size)
{
    assert(vm);

    dictionary* table = (dictionary*)clkwk_allocate(vm, sizeof(dictionary));
    table->capacity = size;
    table->entries = (struct dictionary_entry**)clkwk_allocate(vm, sizeof(struct dictionary_entry*) * size);
    table->count = 0;
    return table;
}

void dictionary_dealloc(dictionary* table, clockwork_vm* vm)
{
    assert(table);
    assert(vm);

    dictionary_purge(table, vm);
    clkwk_freeSize(vm, table->entries, sizeof(struct dictionary_entry*) * table->capacity);
    clkwk_freeSize(vm, table, sizeof(dictionary));
}

void dictionary_set(dictionary* table, clockwork_vm* vm, str* key, object* value)
{
    assert(table);
    assert(vm);
    assert(key);
    assert(value);

    uint32_t bin = _hash_string(key, vm) % table->capacity;
    struct dictionary_entry* next = table->entries[bin];
    struct dictionary_entry* last = NULL;

    while (next != NULL && next->key != NULL && str_compare(key, vm, next->key) > 0)
    {
        last = next;
        next = next->next;
    }

    // Already a value at this location, replace it.
    if (next != NULL && next->key != NULL && str_compare(key, vm, next->key) == 0)
    {
        object_release(next->value, vm);
        next->value = object_retain(value, vm);
    }
    // Create new pair and insert
    else
    {
        struct dictionary_entry* entry = _new_entry(vm, key, value);
        if (next == table->entries[bin])
        {
            entry->next = next;
            table->entries[bin] = entry;
        }
        else if (next == NULL)
        {
            if (last)
            {
                last->next = entry;
            }
        }
        else
        {
            entry->next = next;
            if (last)
            {
                last->next = entry;
            }
        }

        table->count++;
    }
}

object* dictionary_get(dictionary* table, clockwork_vm* vm, str* key)
{
    assert(table);
    assert(vm);
    assert(key);

    uint32_t bin = _hash_string(key, vm) % table->capacity;
    struct dictionary_entry* entry = table->entries[bin];
	while (entry != NULL && entry->key != NULL && str_compare(key, vm, entry->key) > 0)
    {
		entry = entry->next;
	}

    object* value = NULL;

	/* Did we actually find anything? */
	if (entry != NULL && entry->key != NULL && str_compare(key, vm, entry->key) == 0 )
    {
		value = entry->value;
	}

    return value;
}

void dictionary_remove(dictionary* table, clockwork_vm* vm, str* key)
{
    assert(table);
    assert(vm);
    assert(key);

    uint32_t bin = _hash_string(key, vm) % table->capacity;
    struct dictionary_entry* entry = table->entries[bin];
    struct dictionary_entry* previous = NULL;
	while (entry != NULL && entry->key != NULL && str_compare(key, vm, entry->key) > 0)
    {
        previous = entry;
		entry = entry->next;
	}

    if (entry != NULL && entry->key != NULL && str_compare(key, vm, entry->key) == 0)
    {
		object_release(entry->value, vm);
        if (previous)
        {
            previous->next = entry->next;
        }
        clkwk_freeSize(vm, entry, sizeof(struct dictionary_entry));
        table->count--;
	}
}

void dictionary_purge(dictionary* table, clockwork_vm* vm)
{
    assert(table);
    assert(vm);

    for (int i = 0; i < table->capacity; i++)
	{
        if (table->entries[i] != NULL)
        {
            struct dictionary_entry* entry = table->entries[i];
            while (entry)
            {
                object_release(table->entries[i]->value, vm);
                entry = entry->next;
                clkwk_freeSize(vm, entry, sizeof(struct dictionary_entry));
            }
            clkwk_freeSize(vm, table->entries[i], sizeof(struct dictionary_entry));
            table->entries[i] = NULL;
        }
	}
}
