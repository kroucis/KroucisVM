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
#include "binary.h"
#include "binary_internal.h"

#include <stdio.h>
#include <string.h>

uint64_t disassembler_disassembleBinary(clockwork_binary* bin, clockwork_vm* vm, char* disOUT, uint64_t limit)
{
    uint64_t inputIdx = 0;
    uint64_t binLen = clockwork_binary_length(bin);
    const char* data = clockwork_binary_data(bin);
    uint64_t disLen = 0;

#ifdef VERIFY_BINARY_SIGNATURE
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

        data += magic_len;
        binLen -= magic_len;
    }
#endif

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
                disLen += strlen(noop);
                break;
            }
            case clkwk_POP:
            {
                const char* pop = "pop\n";
                sprintf(disOUT, "%s", pop);
                disOUT += strlen(pop);
                disLen += strlen(pop);
                break;
            }
            case clkwk_PUSH_NIL:
            {
                const char* nilstr = "nil\n";
                sprintf(disOUT, "%s", nilstr);
                disOUT += strlen(nilstr);
                disLen += strlen(nilstr);
                break;
            }
            case clkwk_PUSH_TRUE:
            {
                const char* truestr = "true\n";
                sprintf(disOUT, "%s", truestr);
                disOUT += strlen(truestr);
                disLen += strlen(truestr);
                break;
            }
            case clkwk_PUSH_FALSE:
            {
                const char* falsestr = "false\n";
                sprintf(disOUT, "%s", falsestr);
                disOUT += strlen(falsestr);
                disLen += strlen(falsestr);
                break;
            }
            case clkwk_PUSH_SELF:
            {
                const char* selfstr = "self\n";
                sprintf(disOUT, "%s", selfstr);
                disOUT += strlen(selfstr);
                disLen += strlen(selfstr);
                break;
            }
            case clkwk_PUSH_SUPER:
            {
                const char* superstr = "super\n";
                sprintf(disOUT, "%s", superstr);
                disOUT += strlen(superstr);
                disLen += strlen(superstr);
                break;
            }
            case clkwk_PUSH_INT:
            {
                int64_t i;
                memcpy(&i, &data[inputIdx], sizeof(int64_t));

                char temp[255] = { 0 };
                sprintf(disOUT, "push %lld\n", i);
                sprintf(temp, "push %lld\n", i);

                disOUT += strlen(temp);
                disLen += strlen(temp);
                inputIdx += sizeof(int64_t);
                break;
            }
            case clkwk_PUSH_NUMBER:
            {
                double d;
                memcpy(&d, &data[inputIdx], sizeof(double));

//                vm->pc += sizeof(double);
                printf("FLOATING NUMBER NOT IMPLEMENTED YET!\n");
//                exit(EXIT_FAILURE);
                break;
            }
            case clkwk_PUSH_STRING:
            {
                uint64_t len;
                memcpy(&len, &data[inputIdx], sizeof(uint64_t));
                inputIdx += sizeof(uint64_t);

                char string[len + 1];
                memcpy(string, &data[inputIdx], len);
                string[len] = '\0';
                inputIdx += len;

                char temp[255] = { 0 };
                sprintf(disOUT, "push %s\n", string);
                sprintf(temp, "push %s\n", string);

                disOUT += strlen(temp);
                disLen += strlen(temp);
                break;
            }
            case clkwk_PUSH_SYMBOL:
            {
                uint64_t len;
                memcpy(&len, &data[inputIdx], sizeof(uint64_t));
                inputIdx += sizeof(uint64_t);

                char sym[len + 1];
                memcpy(sym, &data[inputIdx], len);
                sym[len] = '\0';
                inputIdx += len;

                char temp[255] = { 0 };
                sprintf(disOUT, "push :%s\n", sym);
                sprintf(temp, "push :%s\n", sym);

                disOUT += strlen(temp);
                disLen += strlen(temp);
                break;
            }
            case clkwk_PUSH_CONSTANT:
            {
                uint64_t len;
                memcpy(&len, &data[inputIdx], sizeof(uint64_t));
                inputIdx += sizeof(uint64_t);

                char constant[len + 1];
                memcpy(constant, &data[inputIdx], len);
                constant[len] = '\0';
                inputIdx += len;

                char temp[255] = { 0 };
                sprintf(disOUT, "push %s\n", constant);
                sprintf(temp, "push %s\n", constant);

                disOUT += strlen(temp);
                disLen += strlen(temp);
                break;
            }
            case clkwk_JUMP:
            {
                uint64_t loc;
                memcpy(&loc, &data[inputIdx], sizeof(uint64_t));

                char temp[255] = { 0 };
                sprintf(disOUT, "jump %llu\n", loc);
                sprintf(temp, "jump %llu\n", loc);

                disOUT += strlen(temp);
                disLen += strlen(temp);
                inputIdx += sizeof(uint64_t);
                break;
            }
            case clkwk_JUMP_IF_TRUE:
            {
                uint64_t loc;
                memcpy(&loc, &data[inputIdx], sizeof(uint64_t));

                char temp[255] = { 0 };
                sprintf(disOUT, "jmpt %llu\n", loc);
                sprintf(temp, "jmpt %llu\n", loc);

                disOUT += strlen(temp);
                disLen += strlen(temp);
                inputIdx += sizeof(uint64_t);
                break;
            }
            case clkwk_JUMP_IF_FALSE:
            {
                uint64_t loc;
                memcpy(&loc, &data[inputIdx], sizeof(uint64_t));

                char temp[255] = { 0 };
                sprintf(disOUT, "jmpf %llu\n", loc);
                sprintf(temp, "jmpf %llu\n", loc);

                disOUT += strlen(temp);
                disLen += strlen(temp);
                inputIdx += sizeof(uint64_t);
                break;
            }
            case clkwk_PUSH_LOCAL:
            {
                local_index idx;
                uint8_t idx_sz = sizeof(local_index);
                memcpy(&idx, &data[inputIdx], idx_sz);
                inputIdx += idx_sz;

                char temp[255] = { 0 };
                sprintf(disOUT, "pushl %d\n", idx);
                sprintf(temp, "pushl %d\n", idx);

                disOUT += strlen(temp);
                disLen += strlen(temp);
                break;
            }
            case clkwk_SET_LOCAL:
            {
                local_index idx;
                uint8_t idx_sz = sizeof(local_index);
                memcpy(&idx, &data[inputIdx], idx_sz);
                inputIdx += idx_sz;

                char temp[255] = { 0 };
                sprintf(disOUT, "setl %d\n", idx);
                sprintf(temp, "setl %d\n", idx);

                disOUT += strlen(temp);
                disLen += strlen(temp);
                break;
            }
            case clkwk_POP_TO_LOCAL:
            {
                local_index idx;
                uint8_t idx_sz = sizeof(local_index);
                memcpy(&idx, &data[inputIdx], idx_sz);
                inputIdx += idx_sz;

                char temp[255] = { 0 };
                sprintf(disOUT, "popl %d\n", idx);
                sprintf(temp, "popl %d\n", idx);

                disOUT += strlen(temp);
                disLen += strlen(temp);
                break;
            }
            case clkwk_DISPATCH:
            {
                uint8_t args = data[inputIdx++];
                uint8_t sym_len = data[inputIdx++];
                char sym[255];
                memcpy(sym, &data[inputIdx], sym_len);
                sym[sym_len] = '\0';
                inputIdx += sym_len;

                char temp[255] = { 0 };
                sprintf(disOUT, "disp %s %d\n", sym, args);
                sprintf(temp, "disp %s %d\n", sym, args);

                disOUT += strlen(temp);
                disLen += strlen(temp);
                break;
            }
            case clkwk_PUSH_CLOCKWORK:
            {
                const char* clkwk = "clkwk\n";
                sprintf(disOUT, "%s", clkwk);
                disOUT += strlen(clkwk);
                disLen += strlen(clkwk);
                break;
            }
            case clkwk_RETURN:
            {
                const char* rtrn = "return\n";
                sprintf(disOUT, "%s", rtrn);
                disOUT += strlen(rtrn);
                disLen += strlen(rtrn);
                break;
            }
            case clkwk_SHUTDOWN:
            {
                const char* end = "end\n";
                sprintf(disOUT, "%s", end);
                disOUT += strlen(end);
                disLen += strlen(end);
                break;
            }
            default:
            {
                printf("ClockworkVM(dis): UNKNOWN OPCODE %c\n", op);
                break;
            }
        }
    }

    return disLen;
}
