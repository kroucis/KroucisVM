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
struct dictionary;
struct clockwork_vm;
struct class;
struct primitive_table;
struct symbol;

typedef struct str sel;
typedef struct object object;

struct object_header
{
    struct class* isa;                  /* 8 */
    object* super;                      /* 8 */
    struct primitive_table* ivars;      /* 8 */
    uint32_t size;                      /* 4 */
    int32_t retainCount;                /* 4 */
    void* extra;                        /* 8 */
                                     /* = 40 */
};

boolean         object_isKindOfClass_native(object*, struct class*);
boolean         object_isMemberOfClass_native(object*, struct class*);

struct class*   object_class(struct clockwork_vm* vm);
uint32_t        object_instanceSize(void);

object*         object_init(struct clockwork_vm*);
object*         object_create_super(struct clockwork_vm*, object*, struct class*, uint32_t);

uint32_t        object_size(object* instance);

void            object_dealloc(object* instance, struct clockwork_vm*);
void            object_setIvar(object* instance, struct clockwork_vm*, struct symbol*, object*);
object*         object_getIvar(object* instance, struct clockwork_vm*, struct symbol*);
object*         object_retain(object* instance, struct clockwork_vm*);
void            object_release(object*, struct clockwork_vm*);
int             object_isNil(object* instance, struct clockwork_vm*);
int             object_isFalse(object* instance, struct clockwork_vm*);
int             object_isTrue(object* instance, struct clockwork_vm*);
boolean         object_respondsToSelector(object* instance, struct clockwork_vm*, struct symbol*);
struct block*   object_findMethod(object* instance, struct clockwork_vm*, struct symbol*);
void            object_setSuper(object*, struct clockwork_vm*, object*);
object*         object_super(object*, struct clockwork_vm*);
struct class*   object_getClass(object*, struct clockwork_vm*);
void            object_printDescription(object*, struct clockwork_vm*);
