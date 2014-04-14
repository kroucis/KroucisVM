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
    class_dealloc(_class, _vm);
    clkwk_dealloc(_vm);

    [super tearDown];
}

- (void) testClassOpen
{
    clkwk_openClass(_vm, "Foo", "Object");

    _class = (class*)clkwk_pop(_vm);
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

#warning CRASH?!
//- (void) testClassAddClassMethod
//{
//    clkwk_openClass(_vm, "Foo", "Object");
//
//    _class = (class*)clkwk_pop(_vm);
//    clkwk_push(_vm, (object*)_class);
//
//    block* blk = block_init_native(_vm, NULL, &test_class_method);
//    clkwk_push(_vm, (object*)blk);
//
//    clkwk_makeStringCstr(_vm, "bar");
//
//    clkwk_dispatch(_vm, "addClassMethod:withImpl:", 2);
//
//    object* klass = (object*)_class;
//    XCTAssertTrue(object_respondsToSelector(klass, _vm, "bar"));
//    clkwk_push(_vm, klass);
//
//    clkwk_push(_vm, clkwk_getConstant(_vm, "Foo"));
//    clkwk_dispatch(_vm, "bar", 0);
//
//    object* tru = clkwk_pop(_vm);
//    XCTAssertTrue(object_isTrue(tru, _vm));
//}

- (void) testClassAddInstanceMethod
{
    clkwk_openClass(_vm, "Foo", "Object");

    _class = (class*)clkwk_pop(_vm);
    clkwk_push(_vm, (object*)_class);

    block* blk = block_init_native(_vm, 0, 0, &test_instance_method);
    clkwk_push(_vm, (object*)blk);

    clkwk_makeStringCstr(_vm, "bar");

    clkwk_dispatch(_vm, "addInstanceMethod:withImpl:", 2);

    clkwk_dispatch(_vm, "alloc", 0);
    clkwk_dispatch(_vm, "init", 0);

    object* instance = clkwk_pop(_vm);
    XCTAssertTrue(object_respondsToSelector(instance, _vm, clkwk_getSymbolCstr(_vm, "bar")));
    clkwk_push(_vm, instance);

    clkwk_dispatch(_vm, "bar", 0);

    object* tru = clkwk_pop(_vm);
    XCTAssertTrue(object_isTrue(tru, _vm));

    clkwk_push(_vm, instance);
    clkwk_dispatch(_vm, "release", 0);
}

@end
