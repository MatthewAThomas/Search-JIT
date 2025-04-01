
/*

Using C api to build an "unrolled" version of a BST "has_key" function

2
  1
  3

=

define i1 @search(i32 %val) {
ENTRY:
    %equal2 = icmp eq i32 %val, 2
    br i1 %equal2, label %TRUE, label %DIFF2  
DIFF2:
    %ineq2 = icmp sgt i32 %val, 2
    br i1 %ineq2, label %EQ3, label %EQ1
EQ3:
    %equal3 = icmp eq i32 %val, 3
    br i1 %equal3, label %TRUE, label %FALSE
EQ1:
    %equal1 = icmp eq i32 %val, 1
    br i1 %equal1, label %TRUE, label %FALSE
TRUE:
    ret i1 1
FALSE:
    ret i1 0
}

*/

#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct  {
    LLVMModuleRef mod;
    LLVMTypeRef val;
    LLVMTypeRef contains;
    LLVMValueRef has_key;
} search_attr;

search_attr create_module(void) {
    search_attr sa;

    printf("Creating module\n");

    sa.mod = LLVMModuleCreateWithName("search_module");
    sa.val = LLVMInt32Type();
    sa.contains = LLVMFunctionType(LLVMInt1Type(), &sa.val, 1, 0);
    sa.has_key = LLVMAddFunction(sa.mod, "has_key", sa.contains);

    printf("Created module\n");

    return sa;
}

void build_function(search_attr *sa, LLVMBuilderRef *builder) {
    LLVMBasicBlockRef ENTRY  = LLVMAppendBasicBlock(sa -> has_key, "ENTRY");
    LLVMBasicBlockRef DIFF2 = LLVMAppendBasicBlock(sa -> has_key, "DIFF2");
    LLVMBasicBlockRef EQ3 = LLVMAppendBasicBlock(sa -> has_key, "EQ3");
    LLVMBasicBlockRef EQ1 = LLVMAppendBasicBlock(sa -> has_key, "EQ1");
    LLVMBasicBlockRef TRUE = LLVMAppendBasicBlock(sa -> has_key, "TRUE");
    LLVMBasicBlockRef FALSE = LLVMAppendBasicBlock(sa -> has_key, "FALSE");

    LLVMPositionBuilderAtEnd(*builder, ENTRY);
    LLVMValueRef equal2 = LLVMBuildICmp(*builder, LLVMIntEQ, LLVMGetParam(sa -> has_key, 0), LLVMConstInt(LLVMInt32Type(), 2, true), "equal2");
    LLVMBuildCondBr(*builder, equal2, TRUE, DIFF2);

    LLVMPositionBuilderAtEnd(*builder, DIFF2);
    LLVMValueRef ineq2 = LLVMBuildICmp(*builder, LLVMIntSGT, LLVMGetParam(sa -> has_key, 0), LLVMConstInt(LLVMInt32Type(), 2, true), "ineq2");
    LLVMBuildCondBr(*builder, ineq2, EQ3, EQ1);

    LLVMPositionBuilderAtEnd(*builder, EQ3);
    LLVMValueRef equal3 = LLVMBuildICmp(*builder, LLVMIntEQ, LLVMGetParam(sa -> has_key, 0), LLVMConstInt(LLVMInt32Type(), 3, true), "equal3");
    LLVMBuildCondBr(*builder, equal3, TRUE, FALSE);

    LLVMPositionBuilderAtEnd(*builder, EQ1);
    LLVMValueRef equal1 = LLVMBuildICmp(*builder, LLVMIntEQ, LLVMGetParam(sa -> has_key, 0), LLVMConstInt(LLVMInt32Type(), 1, true), "equal1");
    LLVMBuildCondBr(*builder, equal1, TRUE, FALSE);

    LLVMPositionBuilderAtEnd(*builder, TRUE);
    LLVMBuildRet(*builder, LLVMConstInt(LLVMInt1Type(), 1, true));

    LLVMPositionBuilderAtEnd(*builder, FALSE);
    LLVMBuildRet(*builder, LLVMConstInt(LLVMInt1Type(), 0, true));

    printf("Built has_key function\n");
}

void verify_module(search_attr *sa) {
    char *error = NULL;
    LLVMVerifyModule(sa -> mod, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    printf("Verified module\n");
}

LLVMExecutionEngineRef create_engine(search_attr *sa) {
    char *error;
    LLVMExecutionEngineRef engine;
    error = NULL;
    LLVMLinkInMCJIT();
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();

    printf("Initialized JIT\n");

    if (LLVMCreateExecutionEngineForModule(&engine, sa -> mod, &error) != 0) {
        fprintf(stderr, "failed to create execution engine\n");
        abort();
    }
    if (error) {
        fprintf(stderr, "error: %s\n", error);
        LLVMDisposeMessage(error);
        exit(EXIT_FAILURE);
    }

    printf("JIT created without errors\n");

    return engine;
}

void run_module(search_attr *sa, int32_t val, LLVMExecutionEngineRef *engine) {
    printf("Running has_key\n");
    bool (*has_key)(int) = (bool (*)(int)) LLVMGetFunctionAddress(*engine, "has_key");
    printf("has_key(%d) :=  %d\n", val, (int) has_key(val));
}

void output_bc(search_attr *sa) {
    if (LLVMWriteBitcodeToFile(sa -> mod, "basic_tree.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }
}

void clean_up(LLVMBuilderRef *builder, LLVMExecutionEngineRef *engine) {
    LLVMDisposeBuilder(*builder);
    LLVMDisposeExecutionEngine(*engine);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Not enough arguments passed");
        exit(-1);
    }
    
    search_attr sa = create_module();

    LLVMBuilderRef builder = LLVMCreateBuilder();
    build_function(&sa, &builder);

    output_bc(&sa);

    verify_module(&sa);

    LLVMExecutionEngineRef engine = create_engine(&sa);

    int32_t val = strtoll(argv[1], NULL, 10);
    run_module(&sa, val, &engine);

    clean_up(&builder, &engine);
}