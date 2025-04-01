#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


typedef struct sum_mul_attr {
    LLVMModuleRef m;
    LLVMTypeRef p[4]; // 0,1 args for sum; 2,3 args for mul
    LLVMTypeRef rsum;
    LLVMTypeRef rmul;
    LLVMValueRef sum;
    LLVMValueRef mul;
} sum_mul_attr;

sum_mul_attr create_module(void) {
    sum_mul_attr sa;

    printf("Creating module\n");

    sa.m = LLVMModuleCreateWithName("my_module");
    sa.p[0] = LLVMInt32Type();
    sa.p[1] = LLVMInt32Type();
    sa.p[2] = LLVMInt32Type();
    sa.p[3] = LLVMInt32Type();
    sa.rsum = LLVMFunctionType(LLVMInt32Type(), sa.p, 2, 0);
    sa.rmul = LLVMFunctionType(LLVMInt32Type(), &sa.p[2], 2, 0);
    sa.sum = LLVMAddFunction(sa.m, "sum", sa.rsum);
    sa.mul = LLVMAddFunction(sa.m, "mul", sa.rmul);

    printf("Created module\n");

    return sa;
}

void build_function(sum_mul_attr *sa, LLVMBuilderRef *builder) {

    LLVMBasicBlockRef sum_entry = LLVMAppendBasicBlock(sa -> sum, "entry");
    LLVMPositionBuilderAtEnd(*builder, sum_entry);
    LLVMValueRef sum_tmp = LLVMBuildAdd(*builder, LLVMGetParam(sa -> sum, 0), LLVMGetParam(sa -> sum, 1), "tmp");
    LLVMBuildRet(*builder, sum_tmp);

    LLVMBasicBlockRef mul_entry = LLVMAppendBasicBlock(sa -> mul, "entry");
    LLVMPositionBuilderAtEnd(*builder, mul_entry);
    LLVMValueRef mul_tmp = LLVMBuildMul(*builder, LLVMGetParam(sa -> mul, 0), LLVMGetParam(sa -> mul, 1), "tmp");
    LLVMBuildRet(*builder, mul_tmp);

    printf("Built sum function\n");
}

void verify_module(sum_mul_attr *sa) {
    char *error = NULL;
    LLVMVerifyModule(sa -> m, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    printf("Verified module\n");
}

LLVMExecutionEngineRef create_engine(sum_mul_attr *sa) {
    char *error;
    LLVMExecutionEngineRef engine;
    error = NULL;
    LLVMLinkInMCJIT();
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();

    printf("Initialized JIT\n");

    if (LLVMCreateExecutionEngineForModule(&engine, sa -> m, &error) != 0) {
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

void run_module(sum_mul_attr *sa, int32_t x, int32_t y,  LLVMExecutionEngineRef *engine) {
    printf("Running sum\n");
    int (*sum_func)(int, int) = (int (*)(int, int)) LLVMGetFunctionAddress(*engine, "sum");
    printf("sum(%d, %d) :=  %d\n", x, y, (int) sum_func(x, y));

    printf("Running mul\n");
    int (*mul_func)(int, int) = (int (*)(int, int)) LLVMGetFunctionAddress(*engine, "mul");
    printf("sum(%d, %d) :=  %d\n", x, y, (int) mul_func(x, y));
}

void output_bc(sum_mul_attr *sa) {
    if (LLVMWriteBitcodeToFile(sa -> m, "sum_mul.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }
}

void clean_up(LLVMBuilderRef *builder, LLVMExecutionEngineRef *engine) {
    LLVMDisposeBuilder(*builder);
    LLVMDisposeExecutionEngine(*engine);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Not enough arguments passed");
        exit(-1);
    }
    
    sum_mul_attr sa = create_module();

    LLVMBuilderRef builder = LLVMCreateBuilder();
    build_function(&sa, &builder);

    verify_module(&sa);

    LLVMExecutionEngineRef engine = create_engine(&sa);

    output_bc(&sa);

    int32_t x = strtoll(argv[1], NULL, 10);
    int32_t y = strtoll(argv[2], NULL, 10);
    run_module(&sa, x, y, &engine);

    clean_up(&builder, &engine);
}