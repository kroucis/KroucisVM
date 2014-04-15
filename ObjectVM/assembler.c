//
//  assembler.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-16.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "assembler.h"

#include "vm.h"
#include "str.h"
#include "primitive_table.h"
#include "object.h"
#include "integer.h"

#include "clkwk_debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef const char* assembler_input;
typedef uint64_t input_index;
typedef uint64_t assembler_input_size;
typedef uint64_t assembler_output_size;
typedef char* assembler_output;

struct assembled_binary
{
    assembler_output_size bytes;
    assembler_output binary_data;
};

struct unresolved_label
{
    char label[255];
    uint64_t pc;
};

struct assembler
{
    assembled_binary* binary;
    primitive_table* labels;
    struct unresolved_label unresolvedLabels[10];
    uint8_t unresolvedIndex;
};

enum {
    INT_TYPE = 0,
    NUMBER_TYPE,
    SYMBOL_TYPE,
    STRING_TYPE,
    CONSTANT_TYPE,
    UNRESOLVED_TYPE,
};

struct _result
{
    input_index index;
    union
    {
        double dbl;
        int64_t i;
    } value;
    char sym[255];
    char type;
};

static uint64_t cstr_to_uint64(char* string)
{
    uint64_t value = 0;
    for (int i = 0; i < strlen(string); i++)
	{
		value = (value * 10) + (string[i] - '0');
	}

    return value;
}

//static int64_t cstr_to_int64(char* string)
//{
//   return atoll(string);
//}

static uint64_t read_word(assembler_input input, assembler_input_size length, input_index index, char* word_OUT, uint16_t max_length)
{
    input_index old_idx = index;
    uint32_t counter = 0;
    while (index < length && counter < max_length && !isblank(input[index]) && input[index] != '\n')
    {
        counter++;
        index++;
    }

    memcpy(word_OUT, &input[old_idx], counter);
    word_OUT[counter] = '\0';

    return index;
}

static void write_cstr(const char* value, uint64_t length, assembled_binary* asm_bin_OUT)
{
    memcpy(&asm_bin_OUT->binary_data[asm_bin_OUT->bytes], value, length);
    asm_bin_OUT->bytes += length;
}

static void write_int64(int64_t value, assembled_binary* asm_bin_OUT)
{
    char* val_ptr = (char*)&value;
    write_cstr(val_ptr, sizeof(int64_t), asm_bin_OUT);
}

static void write_float64(double value, assembled_binary* asm_bin_OUT)
{
    char* val_ptr = (char*)&value;
    write_cstr(val_ptr, sizeof(double), asm_bin_OUT);
}

static void write_char(char value, assembled_binary* asm_bin_OUT)
{
    asm_bin_OUT->binary_data[asm_bin_OUT->bytes++] = value;
}

static void write_unsigned_char(unsigned char value, assembled_binary* asm_bin_OUT)
{
    asm_bin_OUT->binary_data[asm_bin_OUT->bytes++] = value;
}

static struct _result read_push_number(assembler_input input, assembler_input_size length, input_index index)
{
    struct _result result;

    char number[100];
    boolean isFloatingPoint = No;
    index = read_word(input, length, index, number, 99);
    int i = 0;
    while (number[i] != '\0')
    {
        if (number[i] == '.')
        {
            isFloatingPoint = Yes;
            break;
        }
        i++;
    }

    if (isFloatingPoint)
    {
        result.type = NUMBER_TYPE;

        double value = atof(number);
        result.value.dbl = value;
    }
    else
    {
        result.type = INT_TYPE;

        int64_t value = atoll(number);
        result.value.i = value;
    }

    result.index = index;

    return result;
}

static struct _result read_push_symbol(assembler_input input, assembler_input_size length, input_index index)
{
    struct _result result;
    index = read_word(input, length, index, result.sym, 254);
    result.index = index;
    return result;
}

static struct _result read_push_constant(assembler_input input, assembler_input_size length, input_index index)
{
    struct _result result;
    index = read_word(input, length, index, result.sym, 254);
    result.index = index;
    return result;
}

static struct _result read_push_label(assembler_input input, assembler_input_size length, input_index index, primitive_table* labels, clockwork_vm* vm)
{
    struct _result result;
    index = read_word(input, length, index, result.sym, 254);
    integer* pc = (integer*)primitive_table_get(labels, vm, result.sym);
    if (!pc)
    {
        printf("Requested label @%s could not be found!\n", result.sym);
    }
    result.value.i = integer_toInt64(pc, vm);
    result.index = index;
    return result;
}

static struct _result read_push(assembler_input input, assembler_input_size length, input_index index, primitive_table* labels, clockwork_vm* vm)
{
    struct _result result;
    if (input[index] == '#')        // Number constant
    {
        index++;
        result = read_push_number(input, length, index);
    }
    else if (input[index] == ':')   // Symbol constant
    {
        index++;
        result = read_push_symbol(input, length, index);
        result.type = SYMBOL_TYPE;
    }
    else if (input[index] == '"')   // String constant
    {
        index++;
        result = read_push_symbol(input, length, index);
        result.type = STRING_TYPE;
    }
    else if (input[index] == '@')   // Label
    {
        index++;
        result = read_push_label(input, length, index, labels, vm);
        result.type = INT_TYPE;
    }
    else                            // Indexed constant (class name, etc.)
    {
        result = read_push_constant(input, length, index);
        result.type = CONSTANT_TYPE;
    }

    return result;
}

static input_index read_label(assembler_input input, assembler_input_size length, input_index index, primitive_table* labels, clockwork_vm* vm, assembled_binary* asm_bin)
{
    char label[255];
    index = read_word(input, length, index, label, 254);

    CLKWK_DBGPRNT("%s -> %lld\n", label, asm_bin->bytes);

    primitive_table_set(labels, vm, label, (object*)integer_init(vm, asm_bin->bytes));

    return index;
}

static struct _result read_jump(assembler_input input, assembler_input_size length, input_index index, primitive_table* labels, clockwork_vm* vm)
{
    struct _result result;

    index++;    // Skip @ in label

    // Use result.sym instead of a temp buffer to save on space.
    index = read_word(input, length, index, result.sym, 254);

    integer* i = (integer*)primitive_table_get(labels, vm, result.sym);
    if (!i)
    {
        result.type = UNRESOLVED_TYPE;
    }
    else
    {
        result.type = INT_TYPE;
        result.value.i = integer_toInt64(i, vm);
    }

    result.index = index;
    return result;
}

static struct _result read_dispatch(assembler_input input, assembler_input_size length, input_index index)
{
    struct _result result;

    index = read_word(input, length, index, result.sym, 254);

    index++;        // Skip space
    char args[4];
    index = read_word(input, length, index, args, 3);
    result.type = (char)atoi(args);     // Reuse type to store arg num

    result.index = index;

    return result;
}

static struct _result read_pushLocal(assembler_input input, assembler_input_size length, input_index index)
{
    struct _result result;

    index = read_word(input, length, index, result.sym, 254);
    result.value.i = cstr_to_uint64(result.sym);
    result.index = index;
    return result;
}

static struct _result read_setLocal(assembler_input input, assembler_input_size length, input_index index)
{
    return read_pushLocal(input, length, index);
}

static input_index read_mneumonic(assembler_input input, assembler_input_size length, input_index index, assembler* state, clockwork_vm* vm)
{
    char mneumonic[100];
    index = read_word(input, length, index, mneumonic, 99);

    if (strcmp(mneumonic, "noop") == 0)
    {
        assembler_noop(state);
    }
    else if (strcmp(mneumonic, "nil") == 0)
    {
        assembler_nil(state);
    }
    else if (strcmp(mneumonic, "true") == 0)
    {
        assembler_true(state);
    }
    else if (strcmp(mneumonic, "false") == 0)
    {
        assembler_false(state);
    }
    else if (strcmp(mneumonic, "self") == 0)
    {
        assembler_self(state);
    }
    else if (strcmp(mneumonic, "super") == 0)
    {
        assembler_super(state);
    }
    else if (strcmp(mneumonic, "push") == 0)
    {
        index++;
        struct _result push_result = read_push(input, length, index, state->labels, vm);
        index = push_result.index;
        switch (push_result.type)
        {
            case INT_TYPE:
            {
                assembler_pushInt(state, push_result.value.i);
                break;
            }
            case NUMBER_TYPE:
            {
                assembler_pushNumber(state, push_result.value.dbl);
                break;
            }
            case SYMBOL_TYPE:
            {
                assembler_pushSymbol(state, push_result.sym);
                break;
            }
            case STRING_TYPE:
            {
                assembler_pushString(state, push_result.sym);
                break;
            }
            case CONSTANT_TYPE:
            {
                assembler_pushConstant(state, push_result.sym);
                break;
            }
            default:
            {
                exit(EXIT_FAILURE);
                break;
            }
        }
    }
    else if (strcmp(mneumonic, "pop") == 0)
    {
        assembler_pop(state);
    }
    else if (strcmp(mneumonic, "disp") == 0)
    {
        index++;    // Skip first space
        struct _result result = read_dispatch(input, length, index);
        index = result.index;
        assembler_dispatch(state, result.sym, result.type);        // .type is actually the number of arguments.
    }
    else if (strcmp(mneumonic, "jump") == 0)
    {
        index++;
        struct _result result = read_jump(input, length, index, state->labels, vm);
        if (result.type == UNRESOLVED_TYPE)
        {
            uint64_t curPC = state->binary->bytes;
            uint64_t startOfUnresolved = curPC + sizeof(uint8_t);
            struct unresolved_label* ul = &(state->unresolvedLabels[state->unresolvedIndex++]);
            ul->pc = startOfUnresolved;
            strcpy(ul->label, result.sym);
            assembler_jump(state, 0);
        }
        else
        {
            assembler_jump(state, result.value.i);
        }
        index = result.index;
    }
    else if (strcmp(mneumonic, "jmpt") == 0)
    {
        index++;
        struct _result result = read_jump(input, length, index, state->labels, vm);
        index = result.index;
        assembler_jumpTrue(state, result.value.i);
    }
    else if (strcmp(mneumonic, "jmpf") == 0)
    {
        index++;
        struct _result result = read_jump(input, length, index, state->labels, vm);
        index = result.index;
        assembler_jumpFalse(state, result.value.i);
    }
    else if (strcmp(mneumonic, "pushl") == 0)
    {
        index++;
        struct _result result = read_pushLocal(input, length, index);
        index = result.index;
        assembler_pushLocal(state, (uint8_t)result.value.i);
    }
    else if (strcmp(mneumonic, "setl") == 0)
    {
        index++;
        struct _result result = read_setLocal(input, length, index);
        index = result.index;
        assembler_setLocal(state, (uint8_t)result.value.i);
    }
    else if (strcmp(mneumonic, "popl") == 0)
    {
        index++;
        struct _result result = read_setLocal(input, length, index);
        index = result.index;
        assembler_popToLocal(state, (uint8_t)result.value.i);
    }
    else if (strcmp(mneumonic, "clkwk") == 0)
    {
        assembler_pushClockwork(state);
    }
    else if (strcmp(mneumonic, "return") == 0)
    {
        assembler_return(state);
    }
    else if (strcmp(mneumonic, "end") == 0)
    {
        assembler_end(state);
    }
    else
    {
        printf("UNKNOWN MNEUMONIC %s!\n", mneumonic);
    }

    return index;
}

static input_index consume_whitespace(assembler_input input, assembler_input_size length, input_index index)
{
    while (isblank(input[index]))
    {
        index++;
    }

    return index;
}

uint64_t assembled_binary_size(assembled_binary* asm_bin)
{
    return asm_bin->bytes;
}

char* assembled_binary_data(assembled_binary* asm_bin)
{
    return asm_bin->binary_data;
}

void assembled_binary_dealloc(assembled_binary* asm_bin, clockwork_vm* vm)
{
    clkwk_free(vm, asm_bin->binary_data);
    clkwk_free(vm, asm_bin);
}

assembled_binary* assembler_assemble_cstr(assembler_input input, assembler_input_size length, clockwork_vm* vm)
{
    assembler* a = assembler_init(vm);

    input_index index = 0;
    while (index < length)
    {
        index = consume_whitespace(input, length, index);
        if (input[index] == '@')
        {
            index++;    // Skip @ for label declaration
            index = read_label(input, length, index, a->labels, vm, a->binary);
        }
        else
        {
            index = read_mneumonic(input, length, index, a, vm);
        }
        index++;
    }

    assembled_binary* binary = a->binary;

    // Resolve unresolved label locations.
    for (int i = 0; i < a->unresolvedIndex; i++)
	{
        struct unresolved_label* ul = &(a->unresolvedLabels[i]);
		uint64_t jumpPCLocation = ul->pc;
        integer* resolvedPCLocation = (integer*)primitive_table_get(a->labels, vm, ul->label);
        if (!resolvedPCLocation)
        {
            printf("Could not resolve label @%s!\n", ul->label);
            exit(EXIT_FAILURE);
        }
        int64_t resolved = integer_toInt64(resolvedPCLocation, vm);

        memcpy(&(binary->binary_data[jumpPCLocation]), &resolved, sizeof(resolved));
	}

    primitive_table_dealloc(a->labels, vm, Yes);
    clkwk_free(vm, a);

    return binary;
}

assembler* assembler_init(struct clockwork_vm* vm)
{
    assembler* a = clkwk_allocate(vm, sizeof(assembler));
    a->binary = clkwk_allocate(vm, sizeof(assembled_binary));
    a->binary->binary_data = clkwk_allocate(vm, 1000);      // ?
    char* magic_byes = "CLKWK1";
    memcpy(a->binary->binary_data, magic_byes, strlen(magic_byes));
    a->binary->bytes = strlen(magic_byes);
    a->labels = primitive_table_init(vm, 5);        // ?

    return a;
}

void assembler_noop(assembler* ar)
{
    CLKWK_DBGPRNT("noop\n");
    write_char((char)clkwk_NOOP, ar->binary);
}

void assembler_pop(assembler* ar)
{
    CLKWK_DBGPRNT("pop\n");
    write_char((char)clkwk_POP, ar->binary);
}

void assembler_nil(assembler* ar)
{
    CLKWK_DBGPRNT("nil\n");
    write_char((char)clkwk_PUSH_NIL, ar->binary);
}

void assembler_true(assembler* ar)
{
    CLKWK_DBGPRNT("true\n");
    write_char((char)clkwk_PUSH_TRUE, ar->binary);
}

void assembler_false(assembler* ar)
{
    CLKWK_DBGPRNT("false\n");
    write_char((char)clkwk_PUSH_FALSE, ar->binary);
}

void assembler_self(assembler* ar)
{
    CLKWK_DBGPRNT("self\n");
    write_char((char)clkwk_PUSH_SELF, ar->binary);
}

void assembler_super(assembler* ar)
{
    CLKWK_DBGPRNT("super\n");
    write_char((char)clkwk_PUSH_SUPER, ar->binary);
}

void assembler_return(assembler* ar)
{
    CLKWK_DBGPRNT("return\n");
    write_char((char)clkwk_RETURN, ar->binary);
}

void assembler_end(assembler* ar)
{
    CLKWK_DBGPRNT("end\n");
    write_char((char)clkwk_SHUTDOWN, ar->binary);
}

void assembler_pushInt(assembler* ar, int64_t value)
{
    CLKWK_DBGPRNT("push #%lld\n", value);
    // Write PUSH_INT instruction
    write_char((char)clkwk_PUSH_INT, ar->binary);
    write_int64(value, ar->binary);
}

void assembler_pushNumber(assembler* ar, double value)
{
    CLKWK_DBGPRNT("push #%1.10f\n", value);
    // Write PUSH_NUMBER instruction
    write_char((char)clkwk_PUSH_NUMBER, ar->binary);
    write_float64(value, ar->binary);
}

void assembler_pushString(assembler* ar, const char* string)
{
    CLKWK_DBGPRNT("push \"%s\n", string);
    write_char((char)clkwk_PUSH_STRING, ar->binary);
    uint64_t len = strlen(string);
    write_int64(len, ar->binary);
    write_cstr(string, len, ar->binary);
}

void assembler_pushSymbol(assembler* ar, const char* sym)
{
    CLKWK_DBGPRNT("push :%s\n", sym);
    write_char((char)clkwk_PUSH_SYMBOL, ar->binary);
    uint64_t len = strlen(sym);
    write_int64(len, ar->binary);
    write_cstr(sym, len, ar->binary);
}

void assembler_pushConstant(assembler* ar, const char* sym)
{
    CLKWK_DBGPRNT("push %s\n", sym);
    write_char((char)clkwk_PUSH_CONSTANT, ar->binary);
    uint64_t len = strlen(sym);
    write_int64(len, ar->binary);
    write_cstr(sym, len, ar->binary);
}

void assembler_jump(assembler* ar, uint64_t loc)
{
    CLKWK_DBGPRNT("jump %llu\n", loc);
    write_char((char)clkwk_JUMP, ar->binary);
    write_int64(loc, ar->binary);
}

void assembler_jumpTrue(assembler* ar, uint64_t loc)
{
    CLKWK_DBGPRNT("jmpt %llu\n", loc);
    write_char((char)clkwk_JUMP_IF_TRUE, ar->binary);
    write_int64(loc, ar->binary);
}

void assembler_jumpFalse(assembler* ar, uint64_t loc)
{
    CLKWK_DBGPRNT("jmpf %llu\n", loc);
    write_char((char)clkwk_JUMP_IF_FALSE, ar->binary);
    write_int64(loc, ar->binary);
}

void assembler_dispatch(assembler* ar, const char* sel, unsigned char args)
{
    CLKWK_DBGPRNT("disp %s %d\n", sel, args);
    write_char((char)clkwk_DISPATCH, ar->binary);
    write_char(args, ar->binary);
    write_char(strlen(sel), ar->binary);
    write_cstr(sel, strlen(sel), ar->binary);
}

void assembler_pushLocal(assembler* ar, uint8_t lcl)
{
    CLKWK_DBGPRNT("pushl %d\n", lcl);
    write_char((char)clkwk_PUSH_LOCAL, ar->binary);
    write_unsigned_char(lcl, ar->binary);
}

void assembler_setLocal(assembler* ar, uint8_t lcl)
{
    CLKWK_DBGPRNT("setl %d\n", lcl);
    write_char((char)clkwk_SET_LOCAL, ar->binary);
    write_unsigned_char(lcl, ar->binary);
}

void assembler_popToLocal(assembler* ar, uint8_t lcl)
{
    CLKWK_DBGPRNT("popl %d\n", lcl);
    write_char((char)clkwk_SET_LOCAL, ar->binary);
    write_unsigned_char(lcl, ar->binary);
}

void assembler_pushClockwork(assembler *ar)
{
    CLKWK_DBGPRNT("clkwk\n");
    write_char((char)clkwk_PUSH_CLOCKWORK, ar->binary);
}
