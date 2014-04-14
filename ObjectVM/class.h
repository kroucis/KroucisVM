//
//  class.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-20.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

struct str;
struct clockwork_vm;
struct block;
struct symbol;

typedef struct class class;

class* class_init(struct clockwork_vm* vm, char* name, char* superclass);
void class_dealloc(class* klass, struct clockwork_vm* vm);

void class_addInstanceMethod(class*, struct clockwork_vm*, const char* name, struct block*);
void class_addClassMethod(class*, struct clockwork_vm*, const char* name, struct block*);
char* class_name(class*, struct clockwork_vm*);
struct block* class_getClassMethod(class*, struct clockwork_vm*, struct symbol* name);
struct block* class_getInstanceMethod(class*, struct clockwork_vm*, struct symbol* name);
struct block* class_findClassMethod(class*, struct clockwork_vm*, struct symbol* name);
