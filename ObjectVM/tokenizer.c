//
//  input_stream.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-24.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "tokenizer.h"

#include "input_stream.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct tokenizer
{
    input_stream* input;
    char* tokenData;
};

typedef int boolean;
const int Yes = 1;
const int No = 0;

static boolean is_closing(char c)
{
    switch (c)
    {
        case '}': case ')': case ']': case ';': case '\n':
        {
            return Yes;
        }
        default:
            break;
    }

    return No;
}

static boolean is_symbol_char(char c)
{
	if (isdigit(c) || isalpha(c))
    {
        return Yes;
    }

	if (isspace(c) || is_closing(c))
    {
        return No;
    }

	return Yes;
}

static boolean is_block(char c)
{
    switch (c)
    {
        case '(': case '{': case ')': case '}':
        {
            return Yes;
        }
        default:
            break;
    }

    return No;
}

static boolean is_operator(char c)
{
    switch (c)
    {
        case '+': case '-': case '*': case '/':
        {
            return Yes;
        }
        default:
            break;
    }

    return No;
}

static token tokenizer_findNext(tokenizer* tizer)
{
    token tok;
    char* buffer = tizer->tokenData;
    tok.data = buffer;
    char c = '\0';
    while (!input_stream_atEnd(tizer->input, 0) && (c = input_stream_consume(tizer->input)) && (isspace(c) || c == '#'))
    {
        if (c == '#') {
			/* read comment */
			while ((c = input_stream_consume(tizer->input)) && (c != '\n')) { }
			if (input_stream_atEnd(tizer->input, 0)){
                break;	/* break if we run into eof */
            }
        }
        // Skip comments and empty lines.
        // Stop at the end and process end of input.
    }

    if (input_stream_atEnd(tizer->input, 0))      // EOF
    {
        tok.type = T_END;
        return tok;
    }
    else if (isalpha(c))        // identifier
    {
        input_stream_back(tizer->input, 1);
        while ((c = input_stream_consume(tizer->input)) && isalnum(c))
        {
            *buffer++ = c;
        }

        if (c == ':')
        {
            *buffer++ = c;
            tok.type = T_NAMECOLON;
        }
        else
        {
            input_stream_back(tizer->input, 1);
            tok.type = T_NAMECONST;
        }
    }
    else if (isdigit(c))        // number
    {
        tok.value.uint = c - '0';
        while ((c = input_stream_consume(tizer->input)) && isdigit(c))
        {
            *buffer++ = c;
            tok.value.uint = (tok.value.uint * 10) + (c - '0');
        }

        // CHECK UINT64 BOUNDS?

        if (c == '.')           // Possible float?
        {
            if ((c = input_stream_consume(tizer->input)) && isdigit(c))
            {
                *buffer++ = '.';
                do
                {
                    *buffer++ = c;
                } while ((c = input_stream_consume(tizer->input)) && isdigit(c));

                if (c)
                {
                    input_stream_back(tizer->input, 1);
                }

                tok.type = T_FLOAT;
                *buffer = '\0';
                tok.value.dbl = atof(buffer);
            }
            else                // Nope, ordinary period
            {
                if (c)
                {
                    input_stream_back(tizer->input, 2);
                }
            }
        }
        else
        {
            tok.type = T_INTEGER;
            input_stream_back(tizer->input, 1);
        }

        // CHECK EXPONENT FORMAT?

    }
    else if (is_block(c))
    {
        switch (c)
        {
            case '{':
            {
                tok.type = T_BLOCK_OPEN;
            } break;
            case '}':
            {
                tok.type = T_BLOCK_CLOSE;
            } break;
            case '(':
            {
                tok.type = T_BLOCK_ARGS_OPEN;
            } break;
            case ')':
            {
                tok.type = T_BLOCK_ARGS_CLOSE;
            } break;
            default:
                break;
        }
    }
    else if (is_operator(c))
    {
        switch (c)
        {
            case '+':
            {
                tok.type = T_OPERATOR_ADD;
            } break;
            case '-':
            {
                tok.type = T_OPERATOR_SUBTRACT;
            } break;
            case '*':
            {
                tok.type = T_OPERATOR_MULTIPLY;
            } break;
            case '/':
            {
                tok.type = T_OPERATOR_DIVIDE;
            } break;
            default:
                break;
        }
    }
    else if (c == ',')
    {
        tok.type = T_SEPARATOR;
    }
    else if (c == '.')          // Symbol
    {
        while ((c = input_stream_consume(tizer->input)) && is_symbol_char(c))
        {
            *buffer++ = c;
        }

        input_stream_back(tizer->input, 1);

        tok.type = T_SYMBOL;
    }
    else if (c == '\'')         // Single-quoted String
    {
        boolean escaped = No;
        while ((c = input_stream_consume(tizer->input)) && (!escaped && c != '\''))
        {
            *buffer++ = c;
            if (c == '\\')
            {
                escaped = Yes;
            }
            else if (escaped)
            {
                escaped = No;
            }
        }

        tok.type = T_SINGLE_STRING;
    }
    // DOUBLE QUOTED STRING?
    else if (is_closing(c))
    {
        tok.type = T_CLOSING;
    }
    else
    {
        // BINARY?
    }

    *buffer = '\0';
    return tok;
}

tokenizer* tokenizer_init(input_stream* input)
{
    tokenizer* tizer = malloc(sizeof(tokenizer));
    tizer->input = input;
    tizer->tokenData = malloc(80);

    return tizer;
}

void tokenizer_dealloc(tokenizer* tizer)
{
    input_stream_dealloc(tizer->input);
    free(tizer->tokenData);
    free(tizer);
}

token tokenizer_next(tokenizer* tizer)
{
    input_stream_takeSnapshot(tizer->input);
    token t = tokenizer_consume(tizer);
    input_stream_resumeSnapshot(tizer->input);
    return t;
}

token tokenizer_consume(tokenizer* tizer)
{
    return tokenizer_findNext(tizer);
}

void tokenizer_error(tokenizer* tizer)
{

}
