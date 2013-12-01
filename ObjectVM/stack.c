//
//  stack.c
//  my_vm
//
//  Created by Kyle Roucis on 13-5-11.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "stack.h"

#include "object.h"

#include <stdio.h>
#include <stdlib.h>

static unsigned int const kMaxStackCount = 32;

struct stack
{
	object* values[kMaxStackCount];
	unsigned int count;
};

stack* stack_init(void)
{
	stack* the_stack = (stack*)malloc(sizeof(stack));
	the_stack->count = 0;
	return the_stack;
}

void stack_push(stack* s, object* val)
{
	s->values[s->count++] = val;
}

object* stack_pop(stack* s)
{
	object* val = s->values[s->count - 1];
	s->count--;
	return val;
}

void stack_purge(stack* s)
{
	s->count = 0;
}

unsigned int stack_count(stack* s)
{
	return s->count;
}

void stack_dealloc(stack* s)
{
	free(s);
}
