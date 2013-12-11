//
//  primitive_table.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-24.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "primitive_table.h"

#include "object.h"
#include "vm.h"
#include "class.h"

#include <string.h>
#include <stdio.h>

struct primitive_table_entry
{
    char* key;
    object* value;
    struct primitive_table_entry* next;
};

struct primitive_table
{
    uint16_t capacity;
    uint16_t count;
    struct primitive_table_entry** entries;
};

static uint32_t hash_string(char* string)
{
    uint32_t hashval = 0;
	int i = 0;

	/* Convert our string to an integer */
	while (hashval < UINT32_MAX && i < strlen(string)) {
		hashval = hashval << 8;
		hashval += string[i];
		i++;
	}

	return hashval;
}

static struct primitive_table_entry* new_entry(clockwork_vm* vm, char* key, object* obj)
{
    struct primitive_table_entry* entry = vm_allocate(vm, sizeof(struct primitive_table_entry));
    entry->key = vm_allocate(vm, strlen(key));
    strcpy(entry->key, key);
    entry->value = obj;
    entry->next = NULL;

    return entry;
}

primitive_table* primitive_table_init(clockwork_vm* vm, uint32_t capacity)
{
    primitive_table* table = vm_allocate(vm, sizeof(primitive_table));
    table->capacity = capacity;
    table->entries = (struct primitive_table_entry**)vm_allocate(vm, sizeof(struct primitive_table_entry*) * table->capacity);

    return table;
}

void primitive_table_dealloc(primitive_table* m_table, clockwork_vm* vm, boolean purge)
{
    if (purge)
    {
        primitive_table_purge(m_table, vm);
    }
    
    vm_free(vm, m_table->entries);
    vm_free(vm, m_table);
}

void primitive_table_set(primitive_table* m_table, clockwork_vm* vm, char* key, object* obj)
{
    uint32_t bin = hash_string(key) % m_table->capacity;
    struct primitive_table_entry* next = m_table->entries[bin];
    struct primitive_table_entry* last = NULL;

    while (next != NULL && next->key != NULL && strcmp(key, next->key) > 0)
    {
        last = next;
        next = next->next;
    }

    // Already a value at this location, replace it.
    if (next != NULL && next->key != NULL && strcmp(key, next->key) == 0)
    {
        vm_push(vm, obj);
        vm_dispatch(vm, "retain", 0);
        vm_pop(vm);
        vm_push(vm, next->value);
        vm_dispatch(vm, "release", 0);
        vm_pop(vm);
        
        next->value = obj;
    }
    // Create new pair and insert
    else
    {
        struct primitive_table_entry* entry = new_entry(vm, key, obj);
        if (next == m_table->entries[bin])
        {
            entry->next = next;
            m_table->entries[bin] = entry;
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

        m_table->count++;
    }
}

object* primitive_table_get(primitive_table* m_table, clockwork_vm* vm, char* key)
{
    uint32_t bin = hash_string(key) % m_table->capacity;
    struct primitive_table_entry* entry = m_table->entries[bin];
	while (entry != NULL && entry->key != NULL && strcmp(key, entry->key) > 0)
    {
		entry = entry->next;
	}

    object* value = NULL;

	/* Did we actually find anything? */
	if (entry != NULL && entry->key != NULL && strcmp(key, entry->key) == 0 )
    {
		value = entry->value;
	}

    return value;
}

void primitive_table_purge(primitive_table* table, clockwork_vm* vm)
{
    if (table->count > 0)
    {
        for (int i = 0; i < table->capacity; i++)
        {
            if (table->entries[i] != NULL)
            {
                struct primitive_table_entry* entry = table->entries[i];
                while (entry)
                {
//                    struct primitive_table_entry* old = entry;
//                    vm_free(vm, entry->value);
//                    entry = entry->next;
//                    vm_free(vm, old);

                    vm_push(vm, entry->value);
                    vm_dispatch(vm, "release", 0);
                    struct primitive_table_entry* old = entry;
                    entry = entry->next;
                    vm_free(vm, old);
                }
                table->entries[i] = NULL;
            }
        }
    }
}

void primitive_table_print(primitive_table* table, clockwork_vm* vm)
{
    printf("{");
    if (table->count > 0)
    {
        printf("\n");
        for (int i = 0; i < table->capacity; i++)
        {
            if (table->entries[i] != NULL)
            {
                printf("\t%s => %s\n", table->entries[i]->key, class_name(object_getClass(table->entries[i]->value, vm), vm));
            }
        }
    }
    printf("}\n");
}

void primitive_table_each(primitive_table* table, clockwork_vm* vm, primitive_table_iterator itr)
{
    if (table->count > 0 && itr)
    {
        for (int i = 0; i < table->capacity; i++)
        {
            if (table->entries[i] != NULL)
            {
                struct primitive_table_entry* entry = table->entries[i];
                while (entry)
                {
                    struct primitive_table_entry* next = entry->next;
                    itr(vm, entry->key, entry->value);
                    entry = next;
                }
            }
        }
    }
}
