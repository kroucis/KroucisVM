//
//  primitive_table.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-24.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include <stdint.h>

struct object;
struct clockwork_vm;

typedef struct primitive_table primitive_table;

primitive_table* primitive_table_init(struct clockwork_vm*, uint32_t capacity);
void primitive_table_dealloc(primitive_table*, struct clockwork_vm*);
void primitive_table_set(primitive_table*, struct clockwork_vm*, char*, struct object*);
struct object* primitive_table_get(primitive_table*, struct clockwork_vm*, char*);
void primitive_table_purge(primitive_table*, struct clockwork_vm*);
void primitive_table_print(primitive_table* table, struct clockwork_vm* vm);
