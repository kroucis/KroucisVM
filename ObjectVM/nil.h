//
//  nil.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-20.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

struct class;
struct object;
struct clockwork_vm;

struct class* nil_class(struct clockwork_vm*);
struct object* nil_instance(struct clockwork_vm*);

int object_isNil(struct object*, struct clockwork_vm*);
