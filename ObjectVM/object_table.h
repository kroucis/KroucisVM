//
//  object_table.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-15.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

struct object;
struct str;
struct clockwork_vm;

typedef struct object_table object_table;

object_table* object_table_init(struct clockwork_vm*, unsigned int);
void object_table_dealloc(object_table*, struct clockwork_vm*);
void object_table_set(object_table*, struct clockwork_vm*, struct str* key, struct object* value);
struct object* object_table_get(object_table*, struct clockwork_vm*, struct str* key);
void object_table_remove(object_table*, struct clockwork_vm*, struct str* key);
void object_table_purge(object_table*, struct clockwork_vm*);
