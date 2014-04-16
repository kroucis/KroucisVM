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
#include <memory.h>

static unsigned int const c_stack_baseCapacity = 10;
static unsigned int const c_stack_growthFactor = 2;

struct stack
{
	object** values;
	uint32_t count;
    uint32_t capacity;
};

stack* stack_init(void)
{
#warning TODO: clkwk_allocate?
	stack* the_stack = malloc(sizeof(stack));
    the_stack->capacity = c_stack_baseCapacity;
    the_stack->values = malloc(sizeof(object*) * the_stack->capacity);
	the_stack->count = 0;
	return the_stack;
}

void stack_push(stack* s, object* val)
{
    if (s->count + 1 >= s->capacity)
    {
        uint32_t oldCap = s->capacity;
        s->capacity *= c_stack_growthFactor;
        object** newStorage = malloc(sizeof(object*) * s->capacity);
        memcpy(newStorage, s->values, oldCap * sizeof(object*));
        free(s->values);
        s->values = newStorage;
    }
	s->values[s->count++] = val;
}

object* stack_pop(stack* s)
{
	object* val = s->values[s->count - 1];
	s->count--;

#warning TODO: SHRINK CAPACITY?
	return val;
}

void stack_purge(stack* s)
{
    free(s->values);
	s->capacity = c_stack_baseCapacity;
    s->values = malloc(sizeof(object*) * s->capacity);
	s->count = 0;
}

unsigned int stack_count(stack* s)
{
	return s->count;
}

void stack_dealloc(stack* s)
{
    free(s->values);
	free(s);
}
