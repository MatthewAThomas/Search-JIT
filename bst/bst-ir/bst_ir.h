#ifndef BST_IR_H
#define BST_IR_H

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
    LLVMValueRef has_val;
    LLVMBuilderRef builder;
    LLVMExecutionEngineRef engine;

    LLVMBasicBlockRef false_block;
    LLVMBasicBlockRef true_block;
} search_attr;

typedef struct bst_ir {
    // node *(*init_tree) (int32_t);
    // bool (*insert) (node *, int32_t);
    // bool (*has_val) (node *, int32_t);
    // void (*print) (node *);

    search_attr (*init_tree) (int32_t);
    void (*insert) (search_attr *, int32_t);
    bool (*has_val) (search_attr *, int32_t);
    void (*clean_up) (search_attr *);
    void (*print) (search_attr *, char *);

} bst_ir;

bst_ir init_bst_ir(void);

#endif // BST_IR_H 