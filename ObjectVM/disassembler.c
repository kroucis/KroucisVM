//
//  disassembler.c
//  ObjectVM
//
//  Created by Kyle Roucis on 14-4-16.
//  Copyright (c) 2014 Kyle Roucis. All rights reserved.
//

#include "disassembler.h"

#include "vm.h"
#include "assembler.h"

#include <stdio.h>
#include <string.h>

uint64_t disassembler_disassembleBinary(assembled_binary* bin, clockwork_vm* vm, char* disOUT, uint64_t limit)
{
    uint64_t inputIdx = 0;
    uint64_t binLen = assembled_binary_size(bin);
    const char* data = assembled_binary_data(bin);

    // Verify binary signature
    {
        char* magic_bytes = "CLKWK1";
        size_t magic_len = strlen(magic_bytes);
        if (binLen < magic_len)
        {
            printf("ClockworkVM Disassembler: Incompatible binary.\n");
            return 0;
        }

        for (int i = 0; i < magic_len; i++)
        {
            if (data[i] != magic_bytes[i])
            {
                printf("ClockworkVM Disassembler: Incompatible binary.\n");
                return 0;
            }
        }

        inputIdx += magic_len;
    }

    while (inputIdx < binLen)
    {
        // Get next opcode
        char op = data[inputIdx++];
        switch (op)
        {
            case clkwk_NOOP:
            {
                const char* noop = "noop\n";
                sprintf(disOUT, "%s", noop);
                disOUT += strlen(noop);
                break;
            }
            case clkwk_POP:
            {
                const char* pop = "pop\n";
                sprintf(disOUT, "%s", pop);
                disOUT += strlen(pop);
                break;
            }
            case clkwk_PUSH_NIL:
            {
                const char* nilstr = "nil\n";
                sprintf(disOUT, "%s", nilstr);
                disOUT += strlen(nilstr);
                break;
            }
            case clkwk_PUSH_TRUE:
            {
                const char* truestr = "true\n";
                sprintf(disOUT, "%s", truestr);
                disOUT += strlen(truestr);
                break;
            }
            case clkwk_PUSH_FALSE:
            {
                const char* falsestr = "false\n";
                sprintf(disOUT, "%s", falsestr);
                disOUT += strlen(falsestr);
                break;
            }
            case clkwk_PUSH_SELF:
            {
                const char* selfstr = "self\n";
                sprintf(disOUT, "%s", selfstr);
                disOUT += strlen(selfstr);
                break;
            }
            case clkwk_PUSH_SUPER:
            {
                const char* superstr = "super\n";
                sprintf(disOUT, "%s", superstr);
                disOUT += strlen(superstr);
                break;
            }
            case clkwk_PUSH_INT:
            {
//                int64_t i;
//                memcpy(&i, &data[vm->pc], sizeof(int64_t));
//                vm->pc += sizeof(int64_t);
//                integer* intObj = integer_init(vm, i);
//
//
//                clkwk_push(vm, (object*)intObj);
                break;
            }
            case clkwk_PUSH_NUMBER:
            {
//                double d;
//                memcpy(&d, &data[vm->pc], sizeof(double));
//                vm->pc += sizeof(double);
//                printf("FLOATING NUMBER NOT IMPLEMENTED YET!\n");
//                exit(EXIT_FAILURE);
                break;
            }
            case clkwk_PUSH_STRING:
            {
//                uint64_t len;
//                memcpy(&len, &data[vm->pc], sizeof(uint64_t));
//                vm->pc += sizeof(uint64_t);
//
//                char string[len + 1];
//                memcpy(string, &data[vm->pc], len);
//                string[len] = '\0';
//                vm->pc += len;
//
//
//                clkwk_makeStringCstr(vm, string);       // Makes and pushes string literal.
                break;
            }
            case clkwk_PUSH_SYMBOL:
            {
//                uint64_t len;
//                memcpy(&len, &data[vm->pc], sizeof(uint64_t));
//                vm->pc += sizeof(uint64_t);
//
//                char sym[len + 1];
//                memcpy(sym, &data[vm->pc], len);
//                sym[len] = '\0';
//                vm->pc += len;
//
//
//                symbol* theSymbol = symbol_table_get(vm->symbols, sym, vm);
//                clkwk_push(vm, (object*)theSymbol);
                break;
            }
            case clkwk_PUSH_CONSTANT:
            {
//                uint64_t len;
//                memcpy(&len, &data[vm->pc], sizeof(uint64_t));
//                vm->pc += sizeof(uint64_t);
//
//                char constant[len + 1];
//                memcpy(constant, &data[vm->pc], len);
//                constant[len] = '\0';
//                vm->pc += len;
//
//                clkwk_pushConst(vm, constant);
                break;
            }
            case clkwk_JUMP:
            {
                uint64_t loc;
                memcpy(&loc, &data[inputIdx], sizeof(uint64_t));

                sprintf(disOUT, "jump %llu\n", loc);
//                disOUT += strl

                inputIdx += sizeof(uint64_t);
                break;
            }
            case clkwk_JUMP_IF_TRUE:
            {
//                object* o = clkwk_pop(vm);
//                if (object_isTrue(o, vm))
//                {
//                    uint64_t loc;
//                    memcpy(&loc, &data[vm->pc], sizeof(uint64_t));
//                    clkwk_jump(vm, loc);
//                }
//                else
//                {
//                    clkwk_jump(vm, vm->pc + sizeof(uint64_t));
//                }
                break;
            }
            case clkwk_JUMP_IF_FALSE:
            {
//                object* o = clkwk_pop(vm);
//                if (object_isFalse(o, vm))
//                {
//                    uint64_t loc;
//                    memcpy(&loc, &data[vm->pc], sizeof(uint64_t));
//                    clkwk_jump(vm, loc);
//                }
//                else
//                {
//                    clkwk_jump(vm, vm->pc + sizeof(uint64_t));
//                }
                break;
            }
            case clkwk_PUSH_LOCAL:
            {
//                local_index idx;
//                uint8_t idx_sz = sizeof(local_index);
//                memcpy(&idx, &data[vm->pc], idx_sz);
//                vm->pc += idx_sz;
//
//                CLKWK_DBGPRNT("PUSH_LOCAL %d\n", idx);
//
//                clkwk_pushLocal(vm, idx);
                break;
            }
            case clkwk_SET_LOCAL:
            {
//                local_index idx;
//                uint8_t idx_sz = sizeof(local_index);
//                memcpy(&idx, &data[vm->pc], idx_sz);
//                vm->pc += idx_sz;
//
//                clkwk_setLocal(vm, idx);
                break;
            }
            case clkwk_POP_TO_LOCAL:
            {
//                local_index idx;
//                uint8_t idx_sz = sizeof(local_index);
//                memcpy(&idx, &data[vm->pc], idx_sz);
//                vm->pc += idx_sz;
//
//                clkwk_popToLocal(vm, idx);
                break;
            }
            case clkwk_DISPATCH:
            {
//                uint8_t args = data[vm->pc++];
//                uint8_t sym_len = data[vm->pc++];
//                char sym[255];
//                memcpy(sym, &data[vm->pc], sym_len);
//                sym[sym_len] = '\0';
//                vm->pc += sym_len;
//
//                clkwk_dispatch(vm, sym, args);
                break;
            }
            case clkwk_PUSH_CLOCKWORK:
            {
                const char* clkwk = "clkwk\n";
                sprintf(disOUT, "%s", clkwk);
                disOUT += strlen(clkwk);
                break;
            }
            case clkwk_RETURN:
            {
//                clkwk_return(vm);

                const char* rtrn = "return\n";
                sprintf(disOUT, "%s", rtrn);
                disOUT += strlen(rtrn);
                break;
            }
            case clkwk_SHUTDOWN:
            {
                const char* end = "end\n";
                sprintf(disOUT, "%s", end);
                disOUT += strlen(end);
//                vm->pc = len;
                break;
            }
            default:
            {
                printf("ClockworkVM: UNKNOWN OPCODE %c\n", op);
                break;
            }
        }
    }
    return 0;
}
