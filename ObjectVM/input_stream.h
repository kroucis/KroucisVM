//
//  input_stream.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-25.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include <stdint.h>
//#include <stdio.h>

typedef struct input_stream input_stream;

input_stream* input_stream_init_cstr(char*, uint64_t);
void input_stream_dealloc(input_stream*);
char input_stream_next(input_stream*);
void input_stream_back(input_stream* stream, uint64_t backamount);
char input_stream_consume(input_stream*);
int input_stream_at_end(input_stream*, uint64_t);
char input_stream_peek(input_stream*, uint64_t);
void input_stream_take_snapshot(input_stream*);
void input_stream_resume_snapshot(input_stream*);
void input_stream_discard_snapshot(input_stream*);
