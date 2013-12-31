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

@end
