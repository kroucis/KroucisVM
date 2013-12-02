//
//  object.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-10.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include <stdint.h>

static int const Yes = 1;
static int const No = 0;
typedef int boolean;

struct str;
struct block;
struct object_table;
struct clockwork_vm;
struct class;
typedef struct str sel;
typedef struct object object;

boolean object_isKindOfClass_native(object*, struct class*);
boolean object_isMemberOfClass_native(object*, struct class*);

struct class* object_class(struct clockwork_vm* vm);

object* object_init(struct clockwork_vm*);
object* object_create_super(struct clockwork_vm*, object*, struct class*, unsigned long);

uint32_t object_size(void);

void        object_dealloc(object* instance, struct clockwork_vm*);
void        object_setIvar(object* instance, struct clockwork_vm*, char*, object*);
object*     object_getIvar(object* instance, struct clockwork_vm*, char*);
object*     object_retain(object* instance, struct clockwork_vm*);
void        object_release(object*, struct clockwork_vm*);
int         object_isNil(object* instance, struct clockwork_vm*);
int         object_isFalse(object* instance, struct clockwork_vm*);
int         object_isTrue(object* instance, struct clockwork_vm*);
boolean         object_respondsToSelector(object* instance, struct clockwork_vm*, char*);
struct block* object_findMethod(object* instance, struct clockwork_vm*, char*);
void object_setSuper(object*, struct clockwork_vm*, object*);
object*     object_super(object*, struct clockwork_vm*);
struct class* object_getClass(object*, struct clockwork_vm*);
void object_printDescription(object*, struct clockwork_vm*);
