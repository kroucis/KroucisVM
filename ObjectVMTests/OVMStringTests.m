//
//  OVMStringTests.m
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

@interface OVMStringTests : XCTestCase

@end

@implementation OVMStringTests
{
    clockwork_vm* _vm;
    str* _string;
}

- (void)setUp
{
    [super setUp];

    _vm = clkwk_init();
}

- (void)tearDown
{
    if (_string)
    {
        clkwk_push(_vm, (object*)_string);
        clkwk_dispatch(_vm, "release", 0);

        _string = NULL;
    }

    clkwk_dealloc(_vm);

    [super tearDown];
}

- (void) testStringAllocInit
{
    _vm = clkwk_init();
    clkwk_pushConst(_vm, "String");

    clkwk_dispatch(_vm, "alloc", 0);
    clkwk_dispatch(_vm, "init", 0);

    _string = (str*)clkwk_pop(_vm);

    XCTAssert(_string);
    XCTAssertTrue(object_isKindOfClass_native((object*)_string, (class*)clkwk_getConstant(_vm, "String")));
}

- (void) testStringHash
{
    _vm = clkwk_init();
    clkwk_pushConst(_vm, "String");

    clkwk_dispatch(_vm, "new", 0);

    _string = (str*)clkwk_pop(_vm);

    assert(_string);
    assert(object_isKindOfClass_native((object*)_string, (class*)clkwk_getConstant(_vm, "String")));

    clkwk_push(_vm, (object*)_string);
    clkwk_dispatch(_vm, "hash", 0);

    integer* hash = (integer*)clkwk_pop(_vm);
    XCTAssertEqual((int64_t)integer_toInt64(hash, _vm), (int64_t)str_hash(_string, _vm));
}

@end
