//
//  token.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-24.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include <stdint.h>

typedef enum token_type
{
    T_NONE = 0,
    T_IDENTIFIER,
    T_INTEGER,
    T_FLOAT,
    T_SINGLE_STRING,
    T_DOUBLE_STRING,
    T_SYMBOL,
    T_NAMECOLON,
    T_NAMECONST,
    T_CLOSING,
    T_BLOCK_OPEN,
    T_BLOCK_CLOSE,
    T_BLOCK_ARGS_OPEN,
    T_BLOCK_ARGS_CLOSE,
    T_SEPARATOR,
    T_OPERATOR_ADD,
    T_OPERATOR_SUBTRACT,
    T_OPERATOR_MULTIPLY,
    T_OPERATOR_DIVIDE,

    T_END,

    TOKEN_COUNT
} token_type;

typedef struct token
{
    token_type type;
    char* data;
    union
    {
        double dbl;
        uint64_t uint;
    } value;
} token;
