//
//  OVMVMTests.m
//  ObjectVM
//
//  Created by Kyle Roucis on 13-12-20.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#import <XCTest/XCTest.h>

#import "vm.h"
#import "integer.h"
#import "object.h"
#import "class.h"
#import "instruction.h"
#import "assembler.h"
#import "array.h"
#import "str.h"
#import "input_stream.h"

@interface OVMVMTests : XCTestCase

@end

@implementation OVMVMTests
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

- (void) testNil
{
    clkwk_pushNil(_vm);

    clkwk_dispatch(_vm, "isNil", 0);

    object* trueObj = clkwk_pop(_vm);
    XCTAssertTrue(object_isTrue(trueObj, _vm));
    clkwk_push(_vm, trueObj);

    clkwk_dispatch(_vm, "isTrue", 0);

    XCTAssertTrue(clkwk_pop(_vm) == trueObj);
}

- (void) testNilMessage
{
    clkwk_pushNil(_vm);
    clkwk_dispatch(_vm, "anything", 0);
    object* nilObj = clkwk_pop(_vm);
    XCTAssertTrue(object_isNil(nilObj, _vm));

    clkwk_pushConst(_vm, "Nil");
    clkwk_dispatch(_vm, "anything", 0);

    XCTAssertTrue(object_isNil(nilObj, _vm));
}

- (void) testForwardCrash
{
    clkwk_pushConst(_vm, "Object");
    clkwk_dispatch(_vm, "new", 0);

    object* obj = clkwk_pop(_vm);

    XCTAssert(obj);
    XCTAssertTrue(object_isKindOfClass_native(obj, (class*)clkwk_getConstant(_vm, "Object")));

    // TODO: Fix this later with exception handling or whatever is cool these days.
//    clkwk_push(_vm, obj);
//    clkwk_dispatch(_vm, "thisShouldCrash", 0);

    clkwk_push(_vm, obj);
    clkwk_dispatch(_vm, "release", 0);
}

// TODO: Move this somewhere else? I'm just trying to put these test *anywhere* at this time.
- (void) testInputStream
{
    char* input = "foo bar { }";

    input_stream* inputStream = input_stream_init_cstr(input, strlen(input));

    XCTAssertTrue(!input_stream_atEnd(inputStream, 0));

    char f = input_stream_consume(inputStream);
    XCTAssertTrue(f == 'f');
    XCTAssertTrue(!input_stream_atEnd(inputStream, 0));

    char o = input_stream_next(inputStream);
    XCTAssertTrue(o == 'o');
    XCTAssertTrue(!input_stream_atEnd(inputStream, 0));

    char la = input_stream_peek(inputStream, 5);
    XCTAssertTrue(la == 'r');
    XCTAssertTrue(!input_stream_atEnd(inputStream, 0));

    o = input_stream_consume(inputStream);
    XCTAssertTrue(o == 'o');
    XCTAssertTrue(!input_stream_atEnd(inputStream, 0));

    input_stream_takeSnapshot(inputStream);
    for (int i = 0; i < 3; i++)
    {
        input_stream_consume(inputStream);
    }
    char a = input_stream_consume(inputStream);
    XCTAssertTrue(a == 'a');
    XCTAssertTrue(!input_stream_atEnd(inputStream, 0));

    input_stream_resumeSnapshot(inputStream);
    o = input_stream_consume(inputStream);
    XCTAssertTrue(o == 'o');
    XCTAssertTrue(!input_stream_atEnd(inputStream, 0));

    input_stream_dealloc(inputStream);
}

// TODO: Move this somewhere else? I'm just trying to put these test *anywhere* at this time.
- (void) testPrints
{
    //    instruction inst = (instruction){ .op = clkwk_PUSH_STRING, .param_count = 1, .params[0] = "Foobar" };
    //    assembler_run_instruction(&inst, vm);
    clkwk_pushStringCstr(_vm, "Foobar");

    clkwk_popPrintln(_vm);

    clkwk_push(_vm, clkwk_getConstant(_vm, "Object"));
    clkwk_dispatch(_vm, "description", 0);
    clkwk_popPrintln(_vm);
}

@end
