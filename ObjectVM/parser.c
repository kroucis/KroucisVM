//
//  parser.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-28.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "parser.h"

#include "tokenizer.h"
#include "ast.h"

#include <stdlib.h>

struct parser
{
    tokenizer* tizer;
    struct ast_node* root;
};

parser* parser_init(tokenizer* tizer)
{
    parser* p = malloc(sizeof(parser));
    p->tizer = tizer;
    p->root = NULL;
    return p;
}

// Start state
struct ast_node* parser_parse(parser* pser)
{
    // OH CRAP TONS OF WORK TO DO HERE

    return pser->root;
}
