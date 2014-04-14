//
//  symbols.h
//  ObjectVM
//
//  Created by Kyle Roucis on 14-4-4.
//  Copyright (c) 2014 Kyle Roucis. All rights reserved.
//

#pragma once

struct clockwork_vm;
struct object;
struct class;

typedef struct symbol symbol;
typedef struct symbol_table symbol_table;

struct class* symbol_class(struct clockwork_vm* vm);

symbol_table* symbol_table_init(struct clockwork_vm* vm);
void symbol_table_dealloc(symbol_table* table, struct clockwork_vm* vm);
symbol* symbol_table_get(symbol_table* table, const char* s, struct clockwork_vm* vm);

char* symbol_cstr(symbol* sym);
