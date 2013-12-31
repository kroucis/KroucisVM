//
//  OVMArrayTests.m
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
#import "array.h"

@interface OVMArrayTests : XCTestCase

@end

@implementation OVMArrayTests
{
    clockwork_vm* _vm;
    array* _ary;
}

- (void)setUp
{
    [super setUp];

    _vm = clkwk_init();
}

- (void)tearDown
{
    if (_ary)
    {
        clkwk_push(_vm, (object*)_ary);
        clkwk_dispatch(_vm, "release", 0);

        _ary = NULL;
    }

    clkwk_dealloc(_vm);

    [super tearDown];
}

- (void) testArrayNative
{
    _ary = array_init(_vm);

    XCTAssert(_ary);
    XCTAssertEqual((uint64_t)array_count(_ary, _vm), (uint64_t)0);

    clkwk_pushNil(_vm);
    object* nilObj = clkwk_pop(_vm);

    array_add(_ary, _vm, nilObj);

    XCTAssertEqual((uint64_t)array_count(_ary, _vm), (uint64_t)1);
    XCTAssertTrue(array_objectAtIndex(_ary, _vm, 0) == nilObj);

    array_removeAtIndex(_ary, _vm, 0);

    XCTAssertEqual((uint64_t)array_count(_ary, _vm), (uint64_t)0);
}

- (void) testArray
{
    clkwk_pushConst(_vm, "Array");
    clkwk_dispatch(_vm, "new", 0);

    _ary = (array*)clkwk_pop(_vm);

    XCTAssert(_ary);
    XCTAssertTrue(object_isKindOfClass_native((object*)_ary, (class*)clkwk_getConstant(_vm, "Array")));

    clkwk_push(_vm, (object*)_ary);
    clkwk_dispatch(_vm, "count", 0);

    integer* count = (integer*)clkwk_pop(_vm);

    XCTAssertEqual((uint64_t)integer_toInt64(count, _vm), (uint64_t)0);

    clkwk_push(_vm, (object*)_ary);
    clkwk_dispatch(_vm, "isEmpty", 0);

    XCTAssertTrue(object_isTrue(clkwk_pop(_vm), _vm));

    clkwk_push(_vm, (object*)_ary);
    clkwk_pushNil(_vm);
    clkwk_dispatch(_vm, "add:", 1);

    clkwk_dispatch(_vm, "count", 0);

    count = (integer*)clkwk_pop(_vm);

    XCTAssertEqual((uint64_t)integer_toInt64(count, _vm), (uint64_t)1);

    clkwk_push(_vm, (object*)_ary);
    clkwk_dispatch(_vm, "isEmpty", 0);

    XCTAssertTrue(object_isFalse(clkwk_pop(_vm), _vm));

    clkwk_push(_vm, (object*)_ary);
    clkwk_pushNil(_vm);
    clkwk_dispatch(_vm, "contains:", 1);

    object* contains = clkwk_pop(_vm);
    XCTAssertTrue(object_isTrue(contains, _vm));

    clkwk_push(_vm, (object*)_ary);
    clkwk_pushNil(_vm);
    clkwk_dispatch(_vm, "indexOf:", 1);

    integer* idx = (integer*)clkwk_pop(_vm);
    XCTAssertEqual((uint64_t)integer_toInt64(idx, _vm), (uint64_t)0);

    clkwk_push(_vm, (object*)_ary);
    clkwk_pushTrue(_vm);
    clkwk_dispatch(_vm, "indexOf:", 1);

    idx = (integer*)clkwk_pop(_vm);
    XCTAssertTrue(object_isNil((object*)idx, _vm));
}

@end
