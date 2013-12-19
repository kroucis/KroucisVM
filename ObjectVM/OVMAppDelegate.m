//
//  OVMAppDelegate.m
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-10.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#import "OVMAppDelegate.h"

#import "vm.h"
#import "str.h"
#import "object.h"
#import "class.h"
#import "integer.h"
#import "tokenizer.h"
#import "input_stream.h"
#import "assembler.h"
#import "block.h"
#import "array.h"

#import "parser.h"
#import "ast.h"

#import <string.h>

static void test_class_method(object* instance, clockwork_vm* vm)
{
    vm_pushTrue(vm);
}

static void test_init_with_args_method(object* instance, clockwork_vm* vm)
{
    vm_pushSuper(vm);
    vm_dispatch(vm, "init", 0);

    vm_pushSelf(vm);
}

@implementation OVMAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [self testInitObject];
    [self testNewObject];
    [self testNewWithArgs];
    [self testNilObject];
    [self testCreateInt];
    [self testNativeArray];
    [self testClockworkArray];
    [self testCreateString];
    [self testPrints];
    [self testOpenClass];
    [self testAddInstanceMethod];
    [self testAddClassMethod];
    [self testIntAdd];
    [self testIntSub];
    [self testIntMul];
    [self testIntDiv];
    [self testInputStream];
    [self testTokenizer];
    [self testNilMessage];

    //    [self testForwardCrash];
}

- (void) testSimpleAST
{
    numeric_literal_ast_node* six_node = malloc(sizeof(numeric_literal_ast_node));
    six_node->type = NumericTypeInteger;
    six_node->node_type = NodeTypeNumeric;
    six_node->value.uint = 6;

    numeric_literal_ast_node* two_point_seven_node = malloc(sizeof(numeric_literal_ast_node));
    two_point_seven_node->type = NumericTypeFloat;
    two_point_seven_node->node_type = NodeTypeNumeric;
    two_point_seven_node->value.dbl = 2.7;

    binary_ast_node* add_node = malloc(sizeof(binary_ast_node));
    add_node->node_type = NodeTypeBinaryOp;
    add_node->op = BinaryOperatorAdd;
    add_node->left_operand = (ast_node*)six_node;
    add_node->right_operand = (ast_node*)two_point_seven_node;

    free(add_node);
    free(two_point_seven_node);
    free(six_node);
}

- (void) testAddInstanceMethod
{
    clockwork_vm* vm = vm_init();

    vm_openClass(vm, "Foo", "Object");

    block* blk = block_init_native(vm, NULL, &test_class_method);
    vm_push(vm, (object*)blk);

    vm_makeStringCstr(vm, "bar");

    vm_dispatch(vm, "addInstanceMethod:withImpl:", 2);

    vm_dispatch(vm, "alloc", 0);
    vm_dispatch(vm, "init", 0);

    vm_dispatch(vm, "bar", 0);

    object* tru = vm_pop(vm);
    assert(object_isTrue(tru, vm));
    
    vm_dealloc(vm);
}

- (void) testAddClassMethod
{
    clockwork_vm* vm = vm_init();

    vm_openClass(vm, "Foo", "Object");

    block* blk = block_init_native(vm, NULL, &test_class_method);
    vm_push(vm, (object*)blk);

    vm_makeStringCstr(vm, "bar");

    vm_dispatch(vm, "addClassMethod:withImpl:", 2);

    vm_push(vm, vm_getConstant(vm, "Foo"));
    vm_dispatch(vm, "bar", 0);

    object* tru = vm_pop(vm);
    assert(object_isTrue(tru, vm));

    vm_dealloc(vm);
}

- (void) testOpenClass
{
    clockwork_vm* vm = vm_init();

    vm_openClass(vm, "Foo", "Object");

    class* newClass = (class*)vm_pop(vm);
    assert(newClass);

    class* classObj = (class*)vm_getConstant(vm, "Foo");
    assert(classObj);
    assert(classObj == newClass);

    vm_push(vm, vm_getConstant(vm, "Foo"));
    vm_dispatch(vm, "alloc", 0);
    vm_dispatch(vm, "init", 0);

    object* obj = vm_pop(vm);

    assert(obj);
    assert(object_isKindOfClass_native(obj, (class*)vm_getConstant(vm, "Foo")));
    assert(object_isKindOfClass_native(obj, (class*)vm_getConstant(vm, "Object")));

    vm_push(vm, obj);
    vm_dispatch(vm, "release", 0);

    vm_dealloc(vm);
}

- (void) testPrints
{
    clockwork_vm* vm = vm_init();

    instruction inst = (instruction){ .op = VM_PUSH_STRING, .param_count = 1, .params[0] = "Foobar" };
    assembler_run_instruction(&inst, vm);

    vm_popPrintln(vm);

    vm_push(vm, vm_getConstant(vm, "Object"));
    vm_dispatch(vm, "description", 0);
    vm_popPrintln(vm);
    
    vm_dealloc(vm);
}

- (void) testTokenizer
{
//    char* input = "# xthis is a comment\nFoo bar { (x)\n\twoot:x + 6\n }";
//    input_stream* inputStream = input_stream_init_cstr(input, strlen(input));
//
//    tokenizer* t = tokenizer_init(inputStream);
//
//    token tok = tokenizer_next(t);
//
//    tok = tokenizer_consume(t);
//    tok = tokenizer_consume(t);
//    tok = tokenizer_consume(t);
//    tok = tokenizer_consume(t);
//    tok = tokenizer_consume(t);
//    tok = tokenizer_consume(t);
//    tok = tokenizer_consume(t);
//    tok = tokenizer_consume(t);
//    tok = tokenizer_consume(t);
//    tok = tokenizer_consume(t);
//    tok = tokenizer_consume(t);
//    tok = tokenizer_consume(t);
//    tok = tokenizer_consume(t);
//    assert(tok.type == T_END);
//
//    tokenizer_dealloc(t);
}

- (void) testInputStream
{
    char* input = "foo bar { }";

    input_stream* inputStream = input_stream_init_cstr(input, strlen(input));

    assert(!input_stream_atEnd(inputStream, 0));

    char f = input_stream_consume(inputStream);
    assert(f == 'f');
    assert(!input_stream_atEnd(inputStream, 0));

    char o = input_stream_next(inputStream);
    assert(o == 'o');
    assert(!input_stream_atEnd(inputStream, 0));

    char la = input_stream_peek(inputStream, 5);
    assert(la == 'r');
    assert(!input_stream_atEnd(inputStream, 0));

    o = input_stream_consume(inputStream);
    assert(o == 'o');
    assert(!input_stream_atEnd(inputStream, 0));

    input_stream_takeSnapshot(inputStream);
    for (int i = 0; i < 3; i++)
	{
        input_stream_consume(inputStream);
	}
    char a = input_stream_consume(inputStream);
    assert(a == 'a');
    assert(!input_stream_atEnd(inputStream, 0));

    input_stream_resumeSnapshot(inputStream);
    o = input_stream_consume(inputStream);
    assert(o == 'o');
    assert(!input_stream_atEnd(inputStream, 0));

    input_stream_dealloc(inputStream);
}

- (void) testIntAdd
{
    clockwork_vm* vm = vm_init();

    integer* i = integer_init(vm, 42);
    vm_push(vm, (object*)i);
    vm_makeStringCstr(vm, "add:");
    vm_dispatch(vm, "respondsToSelector:", 1);

    object* torf = vm_pop(vm);
    assert(object_isTrue(torf, vm));

    vm_push(vm, (object*)i);

    integer* x = integer_init(vm, 33);
    vm_push(vm, (object*)x);

    vm_dispatch(vm, "add:", 1);

    object* r = vm_pop(vm);
    assert(object_isKindOfClass_native((object*)r, (class*)vm_getConstant(vm, "Integer")));
    integer* ri = (integer*)r;
    assert(integer_toInt64(ri, vm) == (42 + 33));

    vm_dealloc(vm);
}

- (void) testIntSub
{
    clockwork_vm* vm = vm_init();

    integer* i = integer_init(vm, 42);
    vm_push(vm, (object*)i);
    vm_makeStringCstr(vm, "sub:");
    vm_dispatch(vm, "respondsToSelector:", 1);

    object* torf = vm_pop(vm);
    assert(object_isTrue(torf, vm));

    vm_push(vm, (object*)i);

    integer* x = integer_init(vm, 33);
    vm_push(vm, (object*)x);

    vm_dispatch(vm, "sub:", 1);

    object* r = vm_pop(vm);
    assert(object_isKindOfClass_native((object*)r, (class*)vm_getConstant(vm, "Integer")));
    integer* ri = (integer*)r;
    assert(integer_toInt64(ri, vm) == (42 - 33));
    
    vm_dealloc(vm);
}

- (void) testIntMul
{
    clockwork_vm* vm = vm_init();

    integer* i = integer_init(vm, 42);
    vm_push(vm, (object*)i);
    vm_makeStringCstr(vm, "mul:");
    vm_dispatch(vm, "respondsToSelector:", 1);

    object* torf = vm_pop(vm);
    assert(object_isTrue(torf, vm));

    vm_push(vm, (object*)i);

    integer* x = integer_init(vm, 33);
    vm_push(vm, (object*)x);

    vm_dispatch(vm, "mul:", 1);

    object* r = vm_pop(vm);
    assert(object_isKindOfClass_native((object*)r, (class*)vm_getConstant(vm, "Integer")));
    integer* ri = (integer*)r;
    assert(integer_toInt64(ri, vm) == (42 * 33));

    vm_dealloc(vm);
}

- (void) testIntDiv
{
    clockwork_vm* vm = vm_init();

    integer* i = integer_init(vm, 42);
    vm_push(vm, (object*)i);
    vm_makeStringCstr(vm, "div:");
    vm_dispatch(vm, "respondsToSelector:", 1);

    object* torf = vm_pop(vm);
    assert(object_isTrue(torf, vm));

    vm_push(vm, (object*)i);

    integer* x = integer_init(vm, 33);
    vm_push(vm, (object*)x);

    vm_dispatch(vm, "div:", 1);

    object* r = vm_pop(vm);
    assert(object_isKindOfClass_native((object*)r, (class*)vm_getConstant(vm, "Integer")));
    integer* ri = (integer*)r;
    assert(integer_toInt64(ri, vm) == (42 / 33));

    vm_dealloc(vm);
}

- (void) testCreateInt
{
    clockwork_vm* vm = vm_init();

    integer* i = integer_init(vm, 42);

    assert(i);
    assert(object_isKindOfClass_native((object*)i, (class*)vm_getConstant(vm, "Integer")));
    assert(integer_toInt64(i, vm) == 42);

    instruction inst = (instruction){ .op = VM_PUSH_INT, .param_count = 1, .params[0] = "42" };
    assembler_run_instruction(&inst, vm);

    integer* oi = (integer*)vm_pop(vm);

    assert(oi);
    assert(object_isKindOfClass_native((object*)oi, (class*)vm_getConstant(vm, "Integer")));
    assert(integer_toInt64(i, vm) == integer_toInt64(oi, vm));
    assert(integer_toInt64(oi, vm) == 42);

    vm_dealloc(vm);
}

- (void) testCreateString
{
//    clockwork_vm* vm = vm_init();
//
//    instruction inst = (instruction){ .op = VM_PUSH_STRING, .param_count = 1, .params[0] = "Foobar" };
//    assembler_run_instruction(&inst, vm);
//
//    object* obj = vm_pop(vm);
//
//    assert(obj);
//    assert(object_isKindOfClass_native((object*)obj, (class*)vm_getConstant(vm, "String")));
//
//    str* s = (str*)obj;
//
//    assert(strcmp(str_raw_bytes(s, vm), "Foobar") == 0);
//
//    vm_dealloc(vm);

    clockwork_vm* vm = vm_init();
    vm_pushConst(vm, "String");

    vm_dispatch(vm, "alloc", 0);
    vm_dispatch(vm, "init", 0);

    str* s = (str*)vm_pop(vm);

    assert(s);
    assert(object_isKindOfClass_native((object*)s, (class*)vm_getConstant(vm, "String")));

    vm_push(vm, (object*)s);
    vm_dispatch(vm, "release", 0);

    vm_dealloc(vm);
}

- (void) testNilObject
{
    clockwork_vm* vm = vm_init();

    vm_pushNil(vm);

    vm_dispatch(vm, "isNil", 0);

    object* trueObj = vm_pop(vm);
    vm_push(vm, trueObj);

    vm_dispatch(vm, "isTrue", 0);

    assert(vm_pop(vm) == trueObj);

    vm_dealloc(vm);
}

- (void) testInitObject
{
    clockwork_vm* vm = vm_init();
    vm_pushConst(vm, "Object");

    vm_dispatch(vm, "alloc", 0);
    vm_dispatch(vm, "init", 0);

    object* obj = vm_pop(vm);

    assert(obj);
    assert(object_isKindOfClass_native(obj, (class*)vm_getConstant(vm, "Object")));

    vm_push(vm, obj);
    vm_dispatch(vm, "hash", 0);

    object* hashVal = vm_pop(vm);

    assert(hashVal);
    assert(object_isKindOfClass_native((object*)hashVal, (class*)vm_getConstant(vm, "Integer")));
    assert(integer_toInt64((integer*)hashVal, vm) == (int64_t)obj);

    vm_push(vm, obj);
    vm_dispatch(vm, "release", 0);

    vm_dealloc(vm);
}

- (void) testNewObject
{
    clockwork_vm* vm = vm_init();

    vm_pushConst(vm, "Object");
    vm_dispatch(vm, "new", 0);

    object* obj = vm_pop(vm);

    assert(obj);
    assert(object_isKindOfClass_native(obj, (class*)vm_getConstant(vm, "Object")));

    vm_push(vm, obj);
    vm_dispatch(vm, "hash", 0);

    object* hashVal = vm_pop(vm);

    assert(hashVal);
    assert(object_isKindOfClass_native((object*)hashVal, (class*)vm_getConstant(vm, "Integer")));
    assert(integer_toInt64((integer*)hashVal, vm) == (int64_t)obj);

    vm_push(vm, obj);
    vm_dispatch(vm, "release", 0);

    vm_dealloc(vm);
}

- (void) testNewWithArgs
{
    clockwork_vm* vm = vm_init();

    vm_openClass(vm, "Foo", "Object");

    local_scope* iwa_ls = local_scope_init(vm);
    local_scope_addLocal(iwa_ls, vm, "x");
    block* blk = block_init_native(vm, iwa_ls, &test_init_with_args_method);
    vm_push(vm, (object*)blk);

    vm_makeStringCstr(vm, "initWithArgs:");

    vm_dispatch(vm, "addInstanceMethod:withImpl:", 2);

    vm_pushTrue(vm);
    vm_dispatch(vm, "newWithArgs:", 1);

    object* obj = vm_pop(vm);

    assert(obj);
    assert(object_isKindOfClass_native(obj, (class*)vm_getConstant(vm, "Object")));

    vm_push(vm, obj);
    vm_dispatch(vm, "hash", 0);

    object* hashVal = vm_pop(vm);

    assert(hashVal);
    assert(object_isKindOfClass_native((object*)hashVal, (class*)vm_getConstant(vm, "Integer")));
    assert(integer_toInt64((integer*)hashVal, vm) == (int64_t)obj);

    vm_push(vm, obj);
    vm_dispatch(vm, "release", 0);
    
    vm_dealloc(vm);
}

- (void) testNativeArray
{
    clockwork_vm* vm = vm_init();

    array* a = array_init(vm);

    assert(a);
    assert(array_count(a, vm) == 0);

    vm_pushNil(vm);
    object* nilObj = vm_pop(vm);

    array_add(a, vm, nilObj);

    assert(array_count(a, vm) == 1);
    assert(array_objectAtIndex(a, vm, 0) == nilObj);

    array_removeAtIndex(a, vm, 0);

    assert(array_count(a, vm) == 0);


    vm_dealloc(vm);
}

- (void) testClockworkArray
{
    clockwork_vm* vm = vm_init();

    vm_pushConst(vm, "Array");
    vm_dispatch(vm, "new", 0);

    array* ary = (array*)vm_pop(vm);

    assert(ary);
    assert(object_isKindOfClass_native((object*)ary, (class*)vm_getConstant(vm, "Array")));

    vm_push(vm, (object*)ary);
    vm_dispatch(vm, "count", 0);

    integer* count = (integer*)vm_pop(vm);

    assert(integer_toInt64(count, vm) == 0);

    vm_push(vm, (object*)ary);
    vm_dispatch(vm, "isEmpty", 0);

    assert(object_isTrue(vm_pop(vm), vm));

    vm_push(vm, (object*)ary);
    vm_pushNil(vm);
    vm_dispatch(vm, "add:", 1);

    vm_dispatch(vm, "count", 0);

    count = (integer*)vm_pop(vm);

    assert(integer_toInt64(count, vm) == 1);

    vm_push(vm, (object*)ary);
    vm_dispatch(vm, "isEmpty", 0);

    assert(object_isFalse(vm_pop(vm), vm));

    vm_push(vm, (object*)ary);
    vm_pushNil(vm);
    vm_dispatch(vm, "contains:", 1);

    object* contains = vm_pop(vm);
    assert(object_isTrue(contains, vm));

    vm_push(vm, (object*)ary);
    vm_pushNil(vm);
    vm_dispatch(vm, "indexOf:", 1);

    integer* idx = (integer*)vm_pop(vm);
    assert(integer_toInt64(idx, vm) == 0);

    vm_push(vm, (object*)ary);
    vm_pushTrue(vm);
    vm_dispatch(vm, "indexOf:", 1);

    idx = (integer*)vm_pop(vm);
    assert(object_isNil((object*)idx, vm));

    vm_dealloc(vm);
}

- (void) testForwardCrash
{
    clockwork_vm* vm = vm_init();

    vm_pushConst(vm, "Object");
    vm_dispatch(vm, "new", 0);

    object* obj = vm_pop(vm);

    assert(obj);
    assert(object_isKindOfClass_native(obj, (class*)vm_getConstant(vm, "Object")));

    vm_push(vm, obj);
    vm_dispatch(vm, "thisShouldCrash", 0);

    vm_push(vm, obj);
    vm_dispatch(vm, "release", 0);
    
    vm_dealloc(vm);
}

- (void) testNilMessage
{
    clockwork_vm* vm = vm_init();

    vm_pushNil(vm);
    vm_dispatch(vm, "anything", 0);
    vm_pop(vm);
    vm_pushConst(vm, "Nil");
    vm_dispatch(vm, "anything", 0);

    vm_dealloc(vm);
}

@end
