//
//  object_table.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-15.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "object_table.h"

#include "object.h"
#include "vm.h"
#include "str.h"
#include "class.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

struct object_table_entry
{
    str* key;
    object* value;
    struct object_table_entry* next;
};

struct object_table
{
    uint32_t capacity;
    uint32_t count;
    struct object_table_entry** entries;
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

static struct object_table_entry* _new_entry(clockwork_vm* vm, str* key, object* value)
{
    struct object_table_entry* entry = vm_allocate(vm, sizeof(struct object_table_entry));
    entry->key = str_init_len(vm, str_raw_bytes(key, vm), str_length(key, vm));
    entry->value = object_retain(value, vm);
    entry->next = NULL;
    return entry;
}

object_table* object_table_init(clockwork_vm* vm, unsigned int size)
{
    object_table* table = (object_table*)vm_allocate(vm, sizeof(object_table));
    table->capacity = size;
    table->entries = (struct object_table_entry**)vm_allocate(vm, sizeof(struct object_table_entry*) * size);
    table->count = 0;
    return table;
}

void object_table_dealloc(object_table* table, clockwork_vm* vm)
{
    object_table_purge(table, vm);
    vm_freeSize(vm, table->entries, sizeof(struct object_table_entry*) * table->capacity);
    vm_freeSize(vm, table, sizeof(object_table));
}

void object_table_set(object_table* table, clockwork_vm* vm, str* key, object* value)
{
    uint32_t bin = _hash_string(key, vm) % table->capacity;
    struct object_table_entry* next = table->entries[bin];
    struct object_table_entry* last = NULL;

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
        struct object_table_entry* entry = _new_entry(vm, key, value);
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

object* object_table_get(object_table* table, clockwork_vm* vm, str* key)
{
    uint32_t bin = _hash_string(key, vm) % table->capacity;
    struct object_table_entry* entry = table->entries[bin];
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

void object_table_remove(object_table* table, clockwork_vm* vm, str* key)
{
    uint32_t bin = _hash_string(key, vm) % table->capacity;
    struct object_table_entry* entry = table->entries[bin];
    struct object_table_entry* previous = NULL;
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
        vm_freeSize(vm, entry, sizeof(struct object_table_entry));
        table->count--;
	}
}

void object_table_purge(object_table* table, clockwork_vm* vm)
{
    for (int i = 0; i < table->capacity; i++)
	{
        if (table->entries[i] != NULL)
        {
            struct object_table_entry* entry = table->entries[i];
            while (entry)
            {
                object_release(table->entries[i]->value, vm);
                entry = entry->next;
                vm_freeSize(vm, entry, sizeof(struct object_table_entry));
            }
            vm_freeSize(vm, table->entries[i], sizeof(struct object_table_entry));
            table->entries[i] = NULL;
        }
	}
}
