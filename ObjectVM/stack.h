//
//  stack.h
//  my_vm
//
//  Created by Kyle Roucis on 13-5-11.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#ifndef stack_h
#define stack_h

struct object;

typedef struct stack stack;

stack* stack_init(void);
void stack_push(stack* s, struct object* val);
struct object* stack_pop(stack* s);
struct object* stack_top(stack* s);
void stack_purge(stack* s);
unsigned int stack_count(stack* s);
void stack_dealloc(stack* s);

#endif
