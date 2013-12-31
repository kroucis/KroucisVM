//
//  OVMIntegerTests.m
//  ObjectVM
//
//  Created by Kyle Roucis on 13-12-19.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#import <XCTest/XCTest.h>

#import "vm.h"
#import "integer.h"
#import "object.h"
#import "class.h"
#import "instruction.h"
#import "assembler.h"

@interface OVMIntegerTests : XCTestCase

@end

@implementation OVMIntegerTests
{
    clockwork_vm* _vm;
}

- (void)setUp
{
    [super setUp];

    _vm = clkwk_init();
}

- (void)tearDown
{
    clkwk_dealloc(_vm);

    [super tearDown];
}

- (void) testCreateInt
{
    integer* i = integer_init(_vm, 42);

    XCTAssertTrue(i != NULL);
    XCTAssertTrue(object_isKindOfClass_native((object*)i, (class*)clkwk_getConstant(_vm, "Integer")));
    XCTAssertEqual((int64_t)integer_toInt64(i, _vm), (int64_t)42);

    instruction inst = (instruction){ .op = clkwk_PUSH_INT, .param_count = 1, .params[0] = "42" };
    assembler_run_instruction(&inst, _vm);

    integer* oi = (integer*)clkwk_pop(_vm);

    XCTAssertTrue(oi);
    XCTAssertTrue(object_isKindOfClass_native((object*)oi, (class*)clkwk_getConstant(_vm, "Integer")));
    XCTAssertEqual((int64_t)integer_toInt64(i, _vm), (int64_t)integer_toInt64(oi, _vm));
    XCTAssertEqual((int64_t)integer_toInt64(oi, _vm), (int64_t)42);
}

- (void) testIntAdd
{
    integer* i = integer_init(_vm, 42);
    clkwk_push(_vm, (object*)i);
    clkwk_makeStringCstr(_vm, "add:");
    clkwk_dispatch(_vm, "respondsToSelector:", 1);

    object* torf = clkwk_pop(_vm);
    XCTAssertTrue(object_isTrue(torf, _vm));

    clkwk_push(_vm, (object*)i);

    integer* x = integer_init(_vm, 33);
    clkwk_push(_vm, (object*)x);

    clkwk_dispatch(_vm, "add:", 1);

    object* r = clkwk_pop(_vm);
    XCTAssertTrue(object_isKindOfClass_native((object*)r, (class*)clkwk_getConstant(_vm, "Integer")));
    integer* ri = (integer*)r;
    XCTAssertEqual((int64_t)integer_toInt64(ri, _vm), (int64_t)(42 + 33));
}

- (void) testIntSub
{
    integer* i = integer_init(_vm, 42);
    clkwk_push(_vm, (object*)i);
    clkwk_makeStringCstr(_vm, "sub:");
    clkwk_dispatch(_vm, "respondsToSelector:", 1);

    object* torf = clkwk_pop(_vm);
    XCTAssertTrue(object_isTrue(torf, _vm));

    clkwk_push(_vm, (object*)i);

    integer* x = integer_init(_vm, 33);
    clkwk_push(_vm, (object*)x);

    clkwk_dispatch(_vm, "sub:", 1);

    object* r = clkwk_pop(_vm);
    XCTAssertTrue(object_isKindOfClass_native((object*)r, (class*)clkwk_getConstant(_vm, "Integer")));
    integer* ri = (integer*)r;
    XCTAssertEqual((int64_t)integer_toInt64(ri, _vm), (int64_t)(42 - 33));
}

- (void) testIntMul
{
    integer* i = integer_init(_vm, 42);
    clkwk_push(_vm, (object*)i);
    clkwk_makeStringCstr(_vm, "mul:");
    clkwk_dispatch(_vm, "respondsToSelector:", 1);

    object* torf = clkwk_pop(_vm);
    XCTAssertTrue(object_isTrue(torf, _vm));

    clkwk_push(_vm, (object*)i);

    integer* x = integer_init(_vm, 33);
    clkwk_push(_vm, (object*)x);

    clkwk_dispatch(_vm, "mul:", 1);

    object* r = clkwk_pop(_vm);
    XCTAssertTrue(object_isKindOfClass_native((object*)r, (class*)clkwk_getConstant(_vm, "Integer")));
    integer* ri = (integer*)r;
    XCTAssertEqual((int64_t)integer_toInt64(ri, _vm), (int64_t)(42 * 33));
}

- (void) testIntDiv
{
    integer* i = integer_init(_vm, 42);
    clkwk_push(_vm, (object*)i);
    clkwk_makeStringCstr(_vm, "div:");
    clkwk_dispatch(_vm, "respondsToSelector:", 1);

    object* torf = clkwk_pop(_vm);
    XCTAssertTrue(object_isTrue(torf, _vm));

    clkwk_push(_vm, (object*)i);

    integer* x = integer_init(_vm, 33);
    clkwk_push(_vm, (object*)x);

    clkwk_dispatch(_vm, "div:", 1);

    object* r = clkwk_pop(_vm);
    XCTAssertTrue(object_isKindOfClass_native((object*)r, (class*)clkwk_getConstant(_vm, "Integer")));
    integer* ri = (integer*)r;
    XCTAssertEqual((int64_t)integer_toInt64(ri, _vm), (int64_t)(42 / 33));
}

- (void) testIntLessThan
{
    integer* i = integer_init(_vm, 42);
    clkwk_push(_vm, (object*)i);
    clkwk_makeStringCstr(_vm, "lessThan:");
    clkwk_dispatch(_vm, "respondsToSelector:", 1);

    object* torf = clkwk_pop(_vm);
    XCTAssertTrue(object_isTrue(torf, _vm));

    clkwk_push(_vm, (object*)i);

    integer* x = integer_init(_vm, 33);
    clkwk_push(_vm, (object*)x);

    clkwk_dispatch(_vm, "lessThan:", 1);

    object* r = clkwk_pop(_vm);
    XCTAssertTrue(object_isFalse(r, _vm));

    clkwk_push(_vm, (object*)i);

    integer* y = integer_init(_vm, 58);
    clkwk_push(_vm, (object*)y);

    clkwk_dispatch(_vm, "lessThan:", 1);

    r = clkwk_pop(_vm);
    XCTAssertTrue(object_isTrue(r, _vm));
}

- (void) testIntGreaterThan
{
    integer* i = integer_init(_vm, 42);
    clkwk_push(_vm, (object*)i);
    clkwk_makeStringCstr(_vm, "greaterThan:");
    clkwk_dispatch(_vm, "respondsToSelector:", 1);

    object* torf = clkwk_pop(_vm);
    XCTAssertTrue(object_isTrue(torf, _vm));

    clkwk_push(_vm, (object*)i);

    integer* x = integer_init(_vm, 33);
    clkwk_push(_vm, (object*)x);

    clkwk_dispatch(_vm, "greaterThan:", 1);

    object* r = clkwk_pop(_vm);
    XCTAssertTrue(object_isTrue(r, _vm));

    clkwk_push(_vm, (object*)i);

    integer* y = integer_init(_vm, 58);
    clkwk_push(_vm, (object*)y);

    clkwk_dispatch(_vm, "greaterThan:", 1);

    r = clkwk_pop(_vm);
    XCTAssertTrue(object_isFalse(r, _vm));
}

@end
