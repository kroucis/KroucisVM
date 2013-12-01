//
//  parser.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-28.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

struct tokenizer;
struct ast_node;

typedef struct parser parser;

parser* parser_init(struct tokenizer*);
struct ast_node* parser_parse(parser*);
