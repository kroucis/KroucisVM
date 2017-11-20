//
//  OVMClassTests.m
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
#import "symbols.h"

static void test_class_method(object* instance, clockwork_vm* vm)
{
    clkwk_pushTrue(vm);
    clkwk_return(vm);
}

static void test_instance_method(object* instance, clockwork_vm* vm)
{
    clkwk_pushTrue(vm);
    clkwk_return(vm);
}

@interface OVMClassTests : XCTestCase

@end

@implementation OVMClassTests
{
    clockwork_vm* _vm;
    class* _class;
}

- (void)setUp
{
    [super setUp];

    _vm = clkwk_init();
}

- (void)tearDown
{
    if (_class && _vm)
    {
        class_dealloc(_class, _vm);
    }

    if (_vm)
    {
        clkwk_dealloc(_vm);
    }

    [super tearDown];
}

- (void) testClassOpen
{
    _class = clkwk_openClass(_vm, "Foo", "Object");
    XCTAssert(_class);

    class* classObj = (class*)clkwk_getConstant(_vm, "Foo");
    XCTAssert(classObj);
    XCTAssertTrue(classObj == _class);

    clkwk_push(_vm, clkwk_getConstant(_vm, "Foo"));
    clkwk_dispatch(_vm, "alloc", 0);
    clkwk_dispatch(_vm, "init", 0);

    object* obj = clkwk_pop(_vm);

    XCTAssert(obj);
    XCTAssertTrue(object_isKindOfClass_native(obj, (class*)clkwk_getConstant(_vm, "Foo")));
    XCTAssertTrue(object_isKindOfClass_native(obj, (class*)clkwk_getConstant(_vm, "Object")));

    clkwk_push(_vm, obj);
    clkwk_dispatch(_vm, "release", 0);
}

- (void) testClassAddClassMethod
{
    class* foo_class = clkwk_openClass(_vm, "Foo", "Object");
    XCTAssert(foo_class);
    clkwk_push(_vm, (object*)foo_class);

    symbol* bar_symbol = clkwk_getSymbolCstr(_vm, "bar");
    clkwk_push(_vm, (object*)bar_symbol);

    block* blk = block_init_native(_vm, 0, 0, &test_class_method);
    clkwk_push(_vm, (object*)blk);

    clkwk_dispatch(_vm, "addClassMethod:withImplBlock:", 2);

    XCTAssertTrue(object_respondsToSelector((object*)foo_class, _vm, bar_symbol));

    class* maybe_foo_class = (class*)clkwk_getConstant(_vm, "Foo");
    XCTAssert(maybe_foo_class);
    XCTAssertTrue(foo_class == maybe_foo_class);
    clkwk_push(_vm, (object*)maybe_foo_class);
    clkwk_dispatch(_vm, "bar", 0);

    object* tru = clkwk_pop(_vm);
    XCTAssert(object_isTrue(tru, _vm));
}

- (void) testClassAddInstanceMethod
{
    clockwork_vm* vm = clkwk_init();

    class* foo_class = clkwk_openClass(vm, "Foo", "Object");
    clkwk_push(vm, (object*)foo_class);

    symbol* bar_symbol = clkwk_getSymbolCstr(vm, "bar");
    clkwk_push(vm, (object*)bar_symbol);

    block* blk = block_init_native(vm, 0, 0, &test_class_method);
    clkwk_push(vm, (object*)blk);

    clkwk_dispatch(vm, "addInstanceMethod:withImplBlock:", 2);

    clkwk_dispatch(vm, "alloc", 0);
    clkwk_dispatch(vm, "init", 0);

    clkwk_dispatch(vm, "bar", 0);

    object* tru = clkwk_pop(vm);
    XCTAssert(object_isTrue(tru, vm));

    clkwk_dealloc(vm);
}

@end
