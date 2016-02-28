//
//  disassembler.h
//  ObjectVM
//
//  Created by Kyle Roucis on 14-4-16.
//  Copyright (c) 2014 Kyle Roucis. All rights reserved.
//

#pragma once

#include <stdint.h>

struct clockwork_vm;
struct clockwork_binary;

uint64_t disassembler_disassembleBinary(struct clockwork_binary* bin, struct clockwork_vm* vm, char* disOUT, uint64_t limit);
