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

#import "memory_manager.h"
#import "symbols.h"

#import "primitive_table.h"

#import <string.h>

static void test_class_method(object* instance, clockwork_vm* vm)
{
    clkwk_pushTrue(vm);
}

@implementation OVMAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
//    [self testPrints];
//    [self testAddClassMethod];
//    [self testInputStream];
//    [self testTokenizer];

//    memory_manager* mm = memory_manager_init(1024);


    clockwork_vm* vm = clkwk_init();

    primitive_table* tbl = primitive_table_init(vm, 10);
    primitive_table_dealloc(tbl, vm, Yes);

    FILE* file = fopen("/Users/kyleroucis/Desktop/test.clkwkasm", "r");
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* s = malloc(size);
    fread(s, sizeof(char), size, file);

//    char* s = "push \"foo!\ndisp print 0\npush #5\npush #3\nreturn\nend\n";
    assembled_binary* asm_bin = assembler_assemble_cstr(s, strlen(s), vm);
//    printf("[[----------]]\n");
    clkwk_runBinary(vm, asm_bin);

    assembled_binary_dealloc(asm_bin, vm);

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
    clockwork_vm* vm = clkwk_init();

    clkwk_openClass(vm, "Foo", "Object");

    block* blk = block_init_native(vm, 0, 0, &test_class_method);
    clkwk_push(vm, (object*)blk);

    clkwk_makeStringCstr(vm, "bar");

    clkwk_dispatch(vm, "addInstanceMethod:withImpl:", 2);

    clkwk_dispatch(vm, "alloc", 0);
    clkwk_dispatch(vm, "init", 0);

    clkwk_dispatch(vm, "bar", 0);

    object* tru = clkwk_pop(vm);
    assert(object_isTrue(tru, vm));
    
    clkwk_dealloc(vm);
}

- (void) testAddClassMethod
{
    clockwork_vm* vm = clkwk_init();

    clkwk_openClass(vm, "Foo", "Object");

    block* blk = block_init_native(vm, 0, 0, &test_class_method);
    clkwk_push(vm, (object*)blk);

    clkwk_makeStringCstr(vm, "bar");

    clkwk_dispatch(vm, "addClassMethod:withImpl:", 2);

    clkwk_push(vm, clkwk_getConstant(vm, "Foo"));
    clkwk_dispatch(vm, "bar", 0);

    object* tru = clkwk_pop(vm);
    assert(object_isTrue(tru, vm));

    clkwk_dealloc(vm);
}

- (void) testPrints
{
    clockwork_vm* vm = clkwk_init();

//    instruction inst = (instruction){ .op = clkwk_PUSH_STRING, .param_count = 1, .params[0] = "Foobar" };
//    assembler_run_instruction(&inst, vm);
    clkwk_makeStringCstr(vm, "Foobar");

    clkwk_popPrintln(vm);

    clkwk_push(vm, clkwk_getConstant(vm, "Object"));
    clkwk_dispatch(vm, "description", 0);
    clkwk_popPrintln(vm);
    
    clkwk_dealloc(vm);
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

- (void) testForwardCrash
{
    clockwork_vm* vm = clkwk_init();

    clkwk_pushConst(vm, "Object");
    clkwk_dispatch(vm, "new", 0);

    object* obj = clkwk_pop(vm);

    assert(obj);
    assert(object_isKindOfClass_native(obj, (class*)clkwk_getConstant(vm, "Object")));

    clkwk_push(vm, obj);
    clkwk_dispatch(vm, "thisShouldCrash", 0);

    clkwk_push(vm, obj);
    clkwk_dispatch(vm, "release", 0);
    
    clkwk_dealloc(vm);
}

@end
