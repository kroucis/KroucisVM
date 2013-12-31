//
//  OVMObjectTests.m
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

static void foo_initWithArgs_native(object* instance, clockwork_vm* vm)
{
    clkwk_pushSuper(vm);
    clkwk_dispatch(vm, "init", 0);

    clkwk_pushSelf(vm);
}

@interface OVMObjectTests : XCTestCase

@end

@implementation OVMObjectTests
{
    clockwork_vm* _vm;
    object* _obj;
}

- (void)setUp
{
    [super setUp];

    _vm = clkwk_init();
}

- (void)tearDown
{
    if (_obj)
    {
        clkwk_push(_vm, _obj);
        clkwk_dispatch(_vm, "release", 0);

        _obj = NULL;
    }

    clkwk_dealloc(_vm);

    [super tearDown];
}

- (void) testObjectAllocInit
{
    clkwk_pushConst(_vm, "Object");

    clkwk_dispatch(_vm, "alloc", 0);
    clkwk_dispatch(_vm, "init", 0);

    _obj = clkwk_pop(_vm);

    XCTAssertTrue(_obj != NULL);
    XCTAssertTrue(object_isKindOfClass_native(_obj, (class*)clkwk_getConstant(_vm, "Object")));

    clkwk_push(_vm, _obj);
    clkwk_dispatch(_vm, "hash", 0);

    object* hashVal = clkwk_pop(_vm);

    XCTAssertTrue(hashVal);
    XCTAssertTrue(object_isKindOfClass_native((object*)hashVal, (class*)clkwk_getConstant(_vm, "Integer")));
    XCTAssertEqual((int64_t)integer_toInt64((integer*)hashVal, _vm), (int64_t)_obj);
}

- (void) testObjectNew
{
    clkwk_pushConst(_vm, "Object");
    clkwk_dispatch(_vm, "new", 0);

    _obj = clkwk_pop(_vm);

    XCTAssertTrue(_obj);
    XCTAssertTrue(object_isKindOfClass_native(_obj, (class*)clkwk_getConstant(_vm, "Object")));

    clkwk_push(_vm, _obj);
    clkwk_dispatch(_vm, "hash", 0);

    object* hashVal = clkwk_pop(_vm);

    XCTAssertTrue(hashVal);
    XCTAssertTrue(object_isKindOfClass_native((object*)hashVal, (class*)clkwk_getConstant(_vm, "Integer")));
    XCTAssertEqual((int64_t)integer_toInt64((integer*)hashVal, _vm), (int64_t)_obj);
}

- (void) testObjectNewWithArgs
{
    clkwk_openClass(_vm, "Foo", "Object");

    local_scope* iwa_ls = local_scope_init(_vm);
    local_scope_addLocal(iwa_ls, _vm, "x");
    block* blk = block_init_native(_vm, iwa_ls, &foo_initWithArgs_native);
    clkwk_push(_vm, (object*)blk);

    clkwk_makeStringCstr(_vm, "initWithArgs:");

    clkwk_dispatch(_vm, "addInstanceMethod:withImpl:", 2);

    clkwk_pushTrue(_vm);
    clkwk_dispatch(_vm, "newWithArgs:", 1);

    _obj = clkwk_pop(_vm);

    XCTAssertTrue(_obj);
    XCTAssertTrue(object_isKindOfClass_native(_obj, (class*)clkwk_getConstant(_vm, "Object")));

    clkwk_push(_vm, _obj);
    clkwk_dispatch(_vm, "hash", 0);

    object* hashVal = clkwk_pop(_vm);

    XCTAssertTrue(hashVal);
    XCTAssertTrue(object_isKindOfClass_native((object*)hashVal, (class*)clkwk_getConstant(_vm, "Integer")));
    XCTAssertEqual((int64_t)integer_toInt64((integer*)hashVal, _vm), (int64_t)_obj);
}

@end
