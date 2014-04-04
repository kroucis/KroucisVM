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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef char* assembler_input;
typedef uint64_t input_index;
typedef uint64_t assembler_input_size;
typedef uint64_t assembler_output_size;
typedef char* assembler_output;

struct assembled_binary
{
    assembler_output_size bytes;
    assembler_output binary_data;
};

struct assembler
{
    assembled_binary* binary;
    primitive_table* labels;
};

enum {
    INT_TYPE = 0,
    NUMBER_TYPE,
    SYMBOL_TYPE,
    STRING_TYPE,
    CONSTANT_TYPE,
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

static int64_t cstr_to_int64(char* string)
{
   return atoll(string);
}

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

static void write_cstr(char* value, uint64_t length, assembled_binary* asm_bin_OUT)
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

static uint64_t next_string_length(assembler_input input, assembler_input_size length, input_index index)
{
    uint64_t counter = 0;
    while (index + counter < length && input[index + counter] != '\n')
    {
        counter++;
    }

    return counter;
}

static struct _result read_push(assembler_input input, assembler_input_size length, input_index index)
{
    struct _result result;
    if (input[index] == '#')        // Number constant
    {
        index++;
        result = read_push_number(input, length, index);
    }
    else if (input[index] == ':')   // Symbol constant
    {
//        printf(":");
//        index++;
//        index = read_push_symbol(input, length, index, asm_bin_OUT);
    }
    else if (input[index] == '"')   // String constant
    {
        index++;
        result = read_push_symbol(input, length, index);
        result.type = STRING_TYPE;
    }
    else                            // Indexed constant (class name, etc.)
    {

    }

    return result;
}

static input_index read_label(assembler_input input, assembler_input_size length, input_index index, primitive_table* labels, clockwork_vm* vm, assembled_binary* asm_bin)
{
    char label[255];
    index = read_word(input, length, index, label, 254);

    printf("%s\n", label);

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
    result.value.i = integer_toInt64(i, vm);

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

    index++;    // Skip : in symbol

    index = read_word(input, length, index, result.sym, 254);

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
        struct _result push_result = read_push(input, length, index);
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
                exit(1);
                break;
            }
            default:
            {
                exit(1);
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
        index = result.index;
        assembler_jump(state, result.value.i);
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
            printf("@");
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

    primitive_table_dealloc(a->labels, vm, Yes);
//    clkwk_freeSize(vm, a, sizeof(assembler));
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
    printf("noop\n");
    write_char((char)clkwk_NOOP, ar->binary);
}

void assembler_pop(assembler* ar)
{
    printf("pop\n");
    write_char((char)clkwk_POP, ar->binary);
}

void assembler_nil(assembler* ar)
{
    printf("nil\n");
    write_char((char)clkwk_PUSH_NIL, ar->binary);
}

void assembler_true(assembler* ar)
{
    printf("true\n");
    write_char((char)clkwk_PUSH_TRUE, ar->binary);
}

void assembler_false(assembler* ar)
{
    printf("false\n");
    write_char((char)clkwk_PUSH_FALSE, ar->binary);
}

void assembler_self(assembler* ar)
{
    printf("self\n");
    write_char((char)clkwk_PUSH_SELF, ar->binary);
}

void assembler_super(assembler* ar)
{
    printf("super\n");
    write_char((char)clkwk_PUSH_SUPER, ar->binary);
}

void assembler_return(assembler* ar)
{
    printf("return\n");
    write_char((char)clkwk_RETURN, ar->binary);
}

void assembler_end(assembler* ar)
{
    printf("end\n");
    write_char((char)clkwk_SHUTDOWN, ar->binary);
}

void assembler_pushInt(assembler* ar, int64_t value)
{
    printf("push #%lld\n", value);
    // Write PUSH_INT instruction
    write_char((char)clkwk_PUSH_INT, ar->binary);
    write_int64(value, ar->binary);
}

void assembler_pushNumber(assembler* ar, double value)
{
    printf("push #%1.10f\n", value);
    // Write PUSH_NUMBER instruction
    write_char((char)clkwk_PUSH_NUMBER, ar->binary);
    write_float64(value, ar->binary);
}

void assembler_pushString(assembler* ar, char* sym)
{
    printf("push \"%s\n", sym);
    write_char((char)clkwk_PUSH_STRING, ar->binary);
    uint64_t len = strlen(sym);
    write_int64(len, ar->binary);
    write_cstr(sym, len, ar->binary);
}

void assembler_pushSymbol(assembler* ar, symbol sym)
{
    printf("push :%s\n", sym);
    write_char((char)clkwk_PUSH_SYMBOL, ar->binary);
    char len = (char)strlen(sym);
    write_char(len, ar->binary);
    write_cstr(sym, len, ar->binary);
}

void assembler_jump(assembler* ar, uint64_t loc)
{
    printf("jump %llu\n", loc);
    write_char((char)clkwk_JUMP, ar->binary);
    write_int64(loc, ar->binary);
}

void assembler_jumpTrue(assembler* ar, uint64_t loc)
{
    printf("jmpt %llu\n", loc);
    write_char((char)clkwk_JUMP_IF_TRUE, ar->binary);
    write_int64(loc, ar->binary);
}

void assembler_jumpFalse(assembler* ar, uint64_t loc)
{
    printf("jmpf %llu\n", loc);
    write_char((char)clkwk_JUMP_IF_FALSE, ar->binary);
    write_int64(loc, ar->binary);
}

void assembler_dispatch(assembler* ar, char* sel, unsigned char args)
{
    printf("disp %s %d\n", sel, args);
    write_char((char)clkwk_DISPATCH, ar->binary);
    write_char(args, ar->binary);
    write_char(strlen(sel), ar->binary);
    write_cstr(sel, strlen(sel), ar->binary);
}

void assembler_pushLocal(assembler* ar, uint8_t lcl)
{
    printf("pushl %d\n", lcl);
    write_char((char)clkwk_PUSH_LOCAL, ar->binary);
    write_unsigned_char(lcl, ar->binary);
}

void assembler_setLocal(assembler* ar, uint8_t lcl)
{
    printf("setl %d\n", lcl);
    write_char((char)clkwk_SET_LOCAL, ar->binary);
    write_unsigned_char(lcl, ar->binary);
}

void assembler_popToLocal(assembler* ar, uint8_t lcl)
{
    printf("popl %d\n", lcl);
    write_char((char)clkwk_SET_LOCAL, ar->binary);
    write_unsigned_char(lcl, ar->binary);
}

void assembler_pushClockwork(assembler *ar)
{
    printf("clkwk\n");
    write_char((char)clkwk_PUSH_CLOCKWORK, ar->binary);
}

void assembler_run_instruction(instruction* inst, clockwork_vm* vm)
{
    switch (inst->op)
    {
        case clkwk_NOOP:
        {
            break;
        }
        case clkwk_JUMP:
        {
            uint64_t location = cstr_to_uint64(inst->params[0]);
            clkwk_goto(vm, location);
            break;
        }
        case clkwk_JUMP_IF_FALSE:
        {
            uint64_t location = cstr_to_uint64(inst->params[0]);
            clkwk_gotoIfFalse(vm, location);
            break;
        }
        case clkwk_JUMP_IF_TRUE:
        {
            uint64_t location = cstr_to_uint64(inst->params[0]);
            clkwk_gotoIfTrue(vm, location);
            break;
        }
        case clkwk_POP:
        {
            clkwk_pop(vm);
            break;
        }
        case clkwk_PUSH_NIL:
        {
            clkwk_pushNil(vm);
            break;
        }
        case clkwk_PUSH_FALSE:
        {
            clkwk_pushFalse(vm);
            break;
        }
        case clkwk_PUSH_TRUE:
        {
            clkwk_pushTrue(vm);
            break;
        }
        case clkwk_PUSH_LOCAL:
        {
            clkwk_pushLocal(vm, cstr_to_uint64(inst->params[0]));
            break;
        }
        case clkwk_SET_LOCAL:
        {
            clkwk_setLocal(vm, cstr_to_uint64(inst->params[0]));
            break;
        }
        case clkwk_POP_TO_LOCAL:
        {
            clkwk_popToLocal(vm, cstr_to_uint64(inst->params[0]));
            break;
        }
        case clkwk_PUSH_SELF:
        {
            clkwk_pushSelf(vm);
            break;
        }
        case clkwk_PUSH_SUPER:
        {
            clkwk_pushSuper(vm);
            break;
        }
        case clkwk_PUSH_IVAR:
        {
            clkwk_pushIvar(vm, inst->params[0]);
            break;
        }
        case clkwk_SET_IVAR:
        {
            clkwk_setIvar(vm, inst->params[0]);
            break;
        }
        case clkwk_PRINT:
        {
            clkwk_popPrintln(vm);
            break;
        }
        case clkwk_PUSH_STRING:
        {
            str* s = str_init(vm, inst->params[0]);
            clkwk_push(vm, (object*)s);
            break;
        }
        case clkwk_PUSH_INT:
        {
            integer* i = integer_init(vm, cstr_to_int64(inst->params[0]));
            clkwk_push(vm, (object*)i);
            break;
        }
        case clkwk_PUSH_NUMBER:
        {
#warning IMPLEMENT
            break;
        }
        case clkwk_PUSH_CONSTANT:
        {
#warning IMPLEMENT
            break;
        }
        case clkwk_RETURN:
        {
            clkwk_return(vm);
            break;
        }
        default:
        {
            break;
        }
    }
}

void assembler_run_block(block* block, struct clockwork_vm* vm)
{
    instruction_sequence* iseq = block_instructions(block, vm);
    if (iseq != NULL)
    {
        for (int i = 0; i < iseq->inst_count; i++)
        {
            assembler_run_instruction(&iseq->instructions[i], vm);
        }
    }
    else
    {
        native_block native = block_native(block, vm);
        if (native != NULL)
        {
            native(clkwk_currentSelf(vm), vm);
        }
        else
        {
#warning TODO: Throw and exception?
            printf("NATIVE block WAS NULL!");
        }
    }
}
