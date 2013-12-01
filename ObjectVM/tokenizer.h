//
//  input_stream.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-24.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include "token.h"

struct input_stream;

typedef struct tokenizer tokenizer;

tokenizer* tokenizer_init(struct input_stream*);
void tokenizer_dealloc(tokenizer*);
token tokenizer_next(tokenizer*);
token tokenizer_consume(tokenizer*);
void tokenizer_error(tokenizer*);
