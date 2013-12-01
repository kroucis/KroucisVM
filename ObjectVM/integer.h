//
//  integer.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-24.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include <stdint.h>

struct clockwork_vm;
struct class;

typedef struct numeric numeric;

typedef struct integer integer;

struct class* numeric_class(struct clockwork_vm*);

struct class* integer_class(struct clockwork_vm*);

integer* integer_init(struct clockwork_vm*, int64_t);
int64_t integer_toInt64(integer*, struct clockwork_vm*);
