//
//  array.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-12-6.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include <stdint.h>

struct clockwork_vm;
struct class;
struct object;

typedef struct array array;

struct class* array_class(struct clockwork_vm*);

array* array_init(struct clockwork_vm*);
array* array_initWithObjects(struct clockwork_vm*, struct object**, uint64_t);
uint64_t array_count(array*, struct clockwork_vm*);
struct object* array_objectAtIndex(array*, struct clockwork_vm*, uint64_t);
void array_add(array*, struct clockwork_vm*, struct object*);
void array_remove(array* ary, struct clockwork_vm* vm, struct object* obj);
void array_removeAtIndex(array* ary, struct clockwork_vm* vm, uint64_t idx);
void array_each(array* ary, struct clockwork_vm* vm, void(*itr_func)(uint64_t, struct object*));
