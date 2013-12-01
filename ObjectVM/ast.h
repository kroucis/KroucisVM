//
//  ast.h
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-28.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#pragma once

#include <stdint.h>

typedef enum
{
    NodeTypeSequence = 0,
    NodeTypeAssignment,
    NodeTypeBinaryOp,
    NodeTypeNumeric,
} ast_node_type;

typedef enum
{
    BinaryOperatorAdd = 0,
    BinaryOperatorSubtract,
    BinaryOperatorMultiply,
    BinaryOperatorDivide,
    BinaryOperatorDoubleEquals,
    BinaryOperatorTripleEquals,
} binary_operator;

typedef enum
{
    NumericTypeInteger = 0,
    NumericTypeFloat,
} numeric_type;

/* ---- */

typedef struct
{
    ast_node_type node_type;
} ast_node;

typedef struct
{
    ast_node_type node_type;
    uint64_t count;
    ast_node** sequence;
} sequence_ast_node;

typedef struct
{
    ast_node_type node_type;
    ast_node* destination;
    ast_node* expr;
} assignment_ast_node;

typedef struct
{
    ast_node_type node_type;
    binary_operator op;
    ast_node* left_operand;
    ast_node* right_operand;
} binary_ast_node;

typedef struct
{
    ast_node_type node_type;
    numeric_type type;
    union
    {
        uint64_t uint;
        double dbl;
    } value;
} numeric_literal_ast_node;
