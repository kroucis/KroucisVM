//
//  true_false.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-21.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

struct class;
struct object;
struct clockwork_vm;

struct class* true_class(struct clockwork_vm*);
struct object* true_instance(struct clockwork_vm*);

struct class* false_class(struct clockwork_vm*);
struct object* false_instance(struct clockwork_vm*);

int object_isTrue(struct object*, struct clockwork_vm*);
int object_isFalse(struct object*, struct clockwork_vm*);
