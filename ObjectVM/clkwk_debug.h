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
#   define CLKWK_DBGPRNT(fmt, args...)      fprintf(stdout, "CLKWK: ");fprintf(stdout, fmt, ## args)
#   define CLKWK_MARK_CALL                  CLKWK_DBGPRNT(__FUNCTION__)
#   define CLKWK_DBG_ASSERT(stmnt)          assert((stmnt) != 0)
#else
#   define CLKWK_DBGPRNT(fmt, args...)
#endif

#define CLKWK_ASSERT_NOT_NULL(stmnt)        assert((stmnt) != NULL)
