#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


typedef struct sum_attr {
    LLVMModuleRef m;
    LLVMTypeRef p[2];
    LLVMTypeRef r;
    LLVMValueRef s;
} sum_attr;

sum_attr create_module(void) {
    sum_attr sa;

    printf("Creating module\n");

    sa.m = LLVMModuleCreateWithName("my_module");
    sa.p[0] = LLVMInt32Type();
    sa.p[1] = LLVMInt32Type();
    sa.r = LLVMFunctionType(LLVMInt32Type(), sa.p, 2, 0);
    sa.s = LLVMAddFunction(sa.m, "sum", sa.r);

    printf("Created module\n");

    return sa;
}

LLVMBuilderRef build_function(sum_attr *sa) {
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(sa -> s, "entry");
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, entry);
    LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(sa -> s, 0), LLVMGetParam(sa -> s, 1), "tmp");
    LLVMBuildRet(builder, tmp);

    printf("Built sum function\n");

    return builder;
}

void verify_module(sum_attr *sa) {
    char *error = NULL;
    LLVMVerifyModule(sa -> m, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    printf("Verified module\n");
}

LLVMExecutionEngineRef create_engine(sum_attr *sa) {
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

void run_module(sum_attr *sa, int32_t x, int32_t y,  LLVMExecutionEngineRef *engine) {
    printf("Running module\n");
    int (*sum_func)(int, int) = (int (*)(int, int)) LLVMGetFunctionAddress(*engine, "sum");
    printf("sum(%d, %d) :=  %d\n", x, y, (int) sum_func(x, y));
}

void output_bc(sum_attr *sa) {
    if (LLVMWriteBitcodeToFile(sa -> m, "sum.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }
}

void clean_up(LLVMBuilderRef *builder, LLVMExecutionEngineRef *engine) {
    LLVMDisposeBuilder(*builder);
    LLVMDisposeExecutionEngine(*engine);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Not enough arguments passed to sum");
        exit(-1);
    }
    
    sum_attr sa = create_module();
    LLVMBuilderRef builder = build_function(&sa);
    verify_module(&sa);
    LLVMExecutionEngineRef engine = create_engine(&sa);
    output_bc(&sa);

    int32_t x = strtoll(argv[1], NULL, 10);
    int32_t y = strtoll(argv[2], NULL, 10);
    run_module(&sa, x, y, &engine);

    clean_up(&builder, &engine);
}