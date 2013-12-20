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
    vm_dealloc(_vm);

    [super tearDown];
}

@end
