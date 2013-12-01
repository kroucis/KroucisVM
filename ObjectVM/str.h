//
//  str.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-15.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include <inttypes.h>

struct class;
struct clockwork_vm;

typedef struct str str;

struct class* string_class(struct clockwork_vm*);

str*    str_init(struct clockwork_vm*, const char* const data);
str*    str_init_len(struct clockwork_vm*, const char* const data, uint32_t len);

void    str_dealloc(str*, struct clockwork_vm*);
uint32_t str_length(str*, struct clockwork_vm*);
char* str_raw_bytes(str*, struct clockwork_vm*);
void str_into_cstr(str*, struct clockwork_vm*, char*);
int     str_compare(str*, struct clockwork_vm*, str*);
