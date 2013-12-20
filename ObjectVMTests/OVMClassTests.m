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

    _vm = vm_init();
}

- (void)tearDown
{
    class_dealloc(_class, _vm);
    vm_dealloc(_vm);

    [super tearDown];
}

- (void) testClassOpen
{
    vm_openClass(_vm, "Foo", "Object");

    _class = (class*)vm_pop(_vm);
    XCTAssert(_class);

    class* classObj = (class*)vm_getConstant(_vm, "Foo");
    XCTAssert(classObj);
    XCTAssertTrue(classObj == _class);

    vm_push(_vm, vm_getConstant(_vm, "Foo"));
    vm_dispatch(_vm, "alloc", 0);
    vm_dispatch(_vm, "init", 0);

    object* obj = vm_pop(_vm);

    assert(obj);
    assert(object_isKindOfClass_native(obj, (class*)vm_getConstant(_vm, "Foo")));
    assert(object_isKindOfClass_native(obj, (class*)vm_getConstant(_vm, "Object")));

    vm_push(_vm, obj);
    vm_dispatch(_vm, "release", 0);
}

@end
