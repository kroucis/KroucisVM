//
//  clkwk_debug.h
//  ObjectVM
//
//  Created by Kyle Roucis on 14-4-7.
//  Copyright (c) 2014 Kyle Roucis. All rights reserved.
//

#pragma once

#include <stdio.h>

#define CLKWK_DEBUG

#ifdef CLKWK_DEBUG
#   define CLKWK_PRINT_SIZES
#endif

#ifdef CLKWK_DEBUG
#   define CLKWK_DBGPRNT(fmt, args...)     fprintf(stdout, "CLKWK: ");fprintf(stdout, fmt, ## args)
#else
#   define CLKWK_DBGPRNT(fmt, args...)
#endif
