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

    _vm = vm_init();
}

- (void)tearDown
{
    if (_ary)
    {
        vm_push(_vm, (object*)_ary);
        vm_dispatch(_vm, "release", 0);

        _ary = NULL;
    }

    vm_dealloc(_vm);

    [super tearDown];
}

- (void) testArrayNative
{
    _ary = array_init(_vm);

    XCTAssert(_ary);
    XCTAssertEqual((uint64_t)array_count(_ary, _vm), (uint64_t)0);

    vm_pushNil(_vm);
    object* nilObj = vm_pop(_vm);

    array_add(_ary, _vm, nilObj);

    XCTAssertEqual((uint64_t)array_count(_ary, _vm), (uint64_t)1);
    XCTAssertTrue(array_objectAtIndex(_ary, _vm, 0) == nilObj);

    array_removeAtIndex(_ary, _vm, 0);

    XCTAssertEqual((uint64_t)array_count(_ary, _vm), (uint64_t)0);
}

- (void) testArray
{
    vm_pushConst(_vm, "Array");
    vm_dispatch(_vm, "new", 0);

    _ary = (array*)vm_pop(_vm);

    XCTAssert(_ary);
    XCTAssertTrue(object_isKindOfClass_native((object*)_ary, (class*)vm_getConstant(_vm, "Array")));

    vm_push(_vm, (object*)_ary);
    vm_dispatch(_vm, "count", 0);

    integer* count = (integer*)vm_pop(_vm);

    XCTAssertEqual((uint64_t)integer_toInt64(count, _vm), (uint64_t)0);

    vm_push(_vm, (object*)_ary);
    vm_dispatch(_vm, "isEmpty", 0);

    XCTAssertTrue(object_isTrue(vm_pop(_vm), _vm));

    vm_push(_vm, (object*)_ary);
    vm_pushNil(_vm);
    vm_dispatch(_vm, "add:", 1);

    vm_dispatch(_vm, "count", 0);

    count = (integer*)vm_pop(_vm);

    XCTAssertEqual((uint64_t)integer_toInt64(count, _vm), (uint64_t)1);

    vm_push(_vm, (object*)_ary);
    vm_dispatch(_vm, "isEmpty", 0);

    XCTAssertTrue(object_isFalse(vm_pop(_vm), _vm));

    vm_push(_vm, (object*)_ary);
    vm_pushNil(_vm);
    vm_dispatch(_vm, "contains:", 1);

    object* contains = vm_pop(_vm);
    XCTAssertTrue(object_isTrue(contains, _vm));

    vm_push(_vm, (object*)_ary);
    vm_pushNil(_vm);
    vm_dispatch(_vm, "indexOf:", 1);

    integer* idx = (integer*)vm_pop(_vm);
    XCTAssertEqual((uint64_t)integer_toInt64(idx, _vm), (uint64_t)0);

    vm_push(_vm, (object*)_ary);
    vm_pushTrue(_vm);
    vm_dispatch(_vm, "indexOf:", 1);

    idx = (integer*)vm_pop(_vm);
    XCTAssertTrue(object_isNil((object*)idx, _vm));
}

@end
