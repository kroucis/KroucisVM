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

typedef struct dictionary dictionary;

dictionary* dictionary_init(struct clockwork_vm*, unsigned int);
void dictionary_dealloc(dictionary*, struct clockwork_vm*);
void dictionary_set(dictionary*, struct clockwork_vm*, struct str* key, struct object* value);
struct object* dictionary_get(dictionary*, struct clockwork_vm*, struct str* key);
void dictionary_remove(dictionary*, struct clockwork_vm*, struct str* key);
void dictionary_purge(dictionary*, struct clockwork_vm*);