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
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "bst_ir.h"

// #define DEBUG_BST_IR

static search_attr create_module(void) {
    search_attr sa;

    #ifdef DEBUG_BST_IR
    printf("Creating module\n");
    #endif

    sa.mod = LLVMModuleCreateWithName("search_module");
    sa.val = LLVMInt32Type();
    sa.contains = LLVMFunctionType(LLVMInt1Type(), &sa.val, 1, 0);
    sa.has_val = LLVMAddFunction(sa.mod, "has_val", sa.contains);

    sa.builder = LLVMCreateBuilder();

    #ifdef DEBUG_BST_IR
    printf("Created module\n");
    #endif

    return sa;
}

static void verify_module(search_attr *sa) {
    char *error = NULL;
    LLVMVerifyModule(sa -> mod, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    #ifdef DEBUG_BST_IR
    printf("Verified module\n");
    #endif
}

static void create_engine(search_attr *sa) {
    char *error;
    LLVMExecutionEngineRef engine;
    error = NULL;
    LLVMLinkInMCJIT();
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();

    #ifdef DEBUG_BST_IR
    printf("Initialized JIT\n");
    #endif

    if (LLVMCreateExecutionEngineForModule(&engine, sa -> mod, &error) != 0) {
        fprintf(stderr, "failed to create execution engine\n");
        abort();
    }
    if (error) {
        fprintf(stderr, "error: %s\n", error);
        LLVMDisposeMessage(error);
        exit(EXIT_FAILURE);
    }

    #ifdef DEBUG_BST_IR
    printf("JIT created without errors\n");
    #endif

    sa -> engine = engine;
}


/*--------------------------------Printing-----------------------------------*/

static void output_bc(search_attr *sa, char *file_name) {
    if (LLVMWriteBitcodeToFile(sa -> mod, file_name) != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }
}


/*--------------------------------Insertion------------------------------------*/

#define EQ_NODE_PREFIX "EQ"
#define DIFF_NODE_PREFIX "DIFF"
#define EQUAL_CMP_PREFIX "equal"
#define INEQ_CMP_PREFIX "ineq"

#define MAX_NUM_STRING_LEN 12 // number of digits of longest 32-bit number in decimal (10), 
                              // including a minus sign (+1), and null terminator (+1)
#define MAX_LABEL_PREFIX_LEN 5 // longest label prefix is 'equal' - length 5

static char *create_label(char type, int32_t val) {
    char *label = (char *) malloc(MAX_LABEL_PREFIX_LEN + MAX_NUM_STRING_LEN);
    
    char num_buff[MAX_NUM_STRING_LEN];
    sprintf(num_buff, "%d", val);

    switch (type) {
        case 'E':
        case 'e': // EQ type
            memcpy(label, EQ_NODE_PREFIX, 2);
            memcpy(label + 2, num_buff, strlen(num_buff));
            label[2 + strlen(num_buff)] = '\00';
            break;
        case 'D':
        case 'd':
            memcpy(label, DIFF_NODE_PREFIX, 4);
            memcpy(label + 4, num_buff, strlen(num_buff));
            label[4 + strlen(num_buff)] = '\00';
            break;
        case 'T':
        case 't':
            memcpy(label, EQUAL_CMP_PREFIX, 5);
            memcpy(label + 5, num_buff, strlen(num_buff));
            label[5 + strlen(num_buff)] = '\00';
            break;
        case 'F':
        case 'f':
            memcpy(label, INEQ_CMP_PREFIX, 4);
            memcpy(label + 4, num_buff, strlen(num_buff));
            label[4 + strlen(num_buff)] = '\00';
            break;
    }

    return label;
}

static void __insert_initial(search_attr *sa, int32_t val) {
    LLVMBasicBlockRef ENTRY  = LLVMAppendBasicBlock(sa -> has_val, create_label('e', val));
    LLVMBasicBlockRef DIFF = LLVMAppendBasicBlock(sa -> has_val, create_label('d', val));
    LLVMBasicBlockRef TRUE = LLVMAppendBasicBlock(sa -> has_val, (char *) memcpy(malloc(5), "TRUE", 5));
    LLVMBasicBlockRef FALSE = LLVMAppendBasicBlock(sa -> has_val, (char *) memcpy(malloc(6), "FALSE", 6));

    LLVMPositionBuilderAtEnd(sa -> builder, ENTRY);
    LLVMValueRef equal = LLVMBuildICmp(sa -> builder, LLVMIntEQ,
                         LLVMGetParam(sa -> has_val, 0), LLVMConstInt(LLVMInt32Type(), val, true),
                         create_label('t', val));
    LLVMBuildCondBr(sa -> builder, equal, TRUE, DIFF);

    LLVMPositionBuilderAtEnd(sa -> builder, DIFF);
    LLVMValueRef ineq = LLVMBuildICmp(sa -> builder, LLVMIntSGT,
                        LLVMGetParam(sa -> has_val, 0), LLVMConstInt(LLVMInt32Type(), val, true),
                        create_label('f', val));
    LLVMBuildCondBr(sa -> builder, ineq, FALSE, FALSE);

    LLVMPositionBuilderAtEnd(sa -> builder, TRUE);
    LLVMBuildRet(sa -> builder, LLVMConstInt(LLVMInt1Type(), 1, true));

    LLVMPositionBuilderAtEnd(sa -> builder, FALSE);
    LLVMBuildRet(sa -> builder, LLVMConstInt(LLVMInt1Type(), 0, true));

    sa -> true_block = TRUE;
    sa -> false_block = FALSE;
}

static bool is_DIFF_node(char *label) {
    char buff[5];
    memcpy(buff, label, 4);
    buff[4] = '\00';
    return !strcmp(buff, DIFF_NODE_PREFIX);
}

static bool is_leaf_node(char *llabel, char *rlabel, char *label) {
    bool retval = !strcmp(llabel, "FALSE") && !strcmp(rlabel, "FALSE");
    
    // only DIFF nodes should be able to be leaf nodes. EQ nodes should always point to a DIFF node
    if (retval)
        assert(is_DIFF_node(label));
    
    return retval;
}

static int get_label_val(const char *label) {
    int len = strlen(label);
    char num[MAX_NUM_STRING_LEN];
    int idx = MAX_NUM_STRING_LEN - 1;

    for (int i = len - 1; i >= 0; i--) {
        char d = label[i];
        if (isalpha(d))
            break;
        
        num[idx--] = d;
    }

    // should have read at least 1 digit
    assert(idx < MAX_NUM_STRING_LEN - 1);

    // number should not be larger than max int32 (by magnitude)
    assert(idx >= 0);

    int32_t val = atoi(&num[idx + 1]);
    return val;
}

// returns NULL if val already is a key in the tree
static LLVMBasicBlockRef get_end_node(search_attr *sa, int32_t val) {
    LLVMBasicBlockRef bb = LLVMGetEntryBasicBlock(sa -> has_val);

    LLVMValueRef cmp = LLVMGetLastInstruction(bb);
    LLVMBasicBlockRef llabel_blk = LLVMValueAsBasicBlock(LLVMGetOperand(cmp, 2));
    LLVMBasicBlockRef rlabel_blk = LLVMValueAsBasicBlock(LLVMGetOperand(cmp, 1));
    char *llabel = (char *) LLVMGetBasicBlockName(llabel_blk);
    char *rlabel = (char *) LLVMGetBasicBlockName(rlabel_blk);

    while (true) {
        char *label = (char *) LLVMGetBasicBlockName(bb);
        int label_val = get_label_val(label);
        if (label_val == val)
            return NULL;
        
        if (is_leaf_node(llabel, rlabel, label))
            return bb;
        
        if (is_DIFF_node(label)) {
            if (val < label_val) {
                if (!strcmp(rlabel, "FALSE"))
                    return bb;
                bb = rlabel_blk;
            } else {
                if (!strcmp(llabel, "FALSE"))
                    return bb;
                bb = llabel_blk;
            }
        } else { // is EQ node
            if (!strcmp(llabel, "TRUE")) {
                bb = rlabel_blk;
            } else {
                bb = llabel_blk;
            }
        }

        cmp = LLVMGetLastInstruction(bb);
        llabel_blk = LLVMValueAsBasicBlock(LLVMGetOperand(cmp, 2));
        rlabel_blk = LLVMValueAsBasicBlock(LLVMGetOperand(cmp, 1));
        llabel = (char *) LLVMGetBasicBlockName(llabel_blk);
        rlabel = (char *) LLVMGetBasicBlockName(rlabel_blk);
    }

    return bb;
}

static void append_to_end_node(search_attr *sa, int32_t val, LLVMBasicBlockRef ln) {
    
    // Make new EQ and DIFF basic blocks to be appended
    LLVMBasicBlockRef NEW_EQ  = LLVMAppendBasicBlock(sa -> has_val, create_label('e', val));
    LLVMBasicBlockRef NEW_DIFF = LLVMAppendBasicBlock(sa -> has_val, create_label('d', val));
    LLVMBasicBlockRef FALSE = sa -> false_block;
    LLVMBasicBlockRef TRUE = sa -> true_block;

    LLVMPositionBuilderAtEnd(sa -> builder, NEW_EQ);
    LLVMValueRef equal = LLVMBuildICmp(sa -> builder, LLVMIntEQ,
                         LLVMGetParam(sa -> has_val, 0), LLVMConstInt(LLVMInt32Type(), val, true),
                         create_label('t', val));
    LLVMBuildCondBr(sa -> builder, equal, TRUE, NEW_DIFF);

    LLVMPositionBuilderAtEnd(sa -> builder, NEW_DIFF);
    LLVMValueRef ineq = LLVMBuildICmp(sa -> builder, LLVMIntSGT,
                        LLVMGetParam(sa -> has_val, 0), LLVMConstInt(LLVMInt32Type(), val, true),
                        create_label('f', val));
    LLVMBuildCondBr(sa -> builder, ineq, FALSE, FALSE);

    
    // Prepare to append to ln
    char *label = (char *) LLVMGetBasicBlockName(ln);
    int label_val = get_label_val(label);

    assert(is_DIFF_node(label));

    LLVMValueRef cmp = LLVMGetLastInstruction(ln);
    LLVMValueRef pred = LLVMGetOperand(cmp, 0);
    LLVMBasicBlockRef llabel_blk = LLVMValueAsBasicBlock(LLVMGetOperand(cmp, 2));
    LLVMBasicBlockRef rlabel_blk = LLVMValueAsBasicBlock(LLVMGetOperand(cmp, 1));
    char *llabel = (char *) LLVMGetBasicBlockName(llabel_blk);
    char *rlabel = (char *) LLVMGetBasicBlockName(rlabel_blk);

    LLVMInstructionRemoveFromParent(cmp);
    
    // append
    LLVMValueRef new_cmp;
    LLVMPositionBuilderAtEnd(sa -> builder, ln);

    if (val > label_val) {
        assert(!strcmp(llabel, "FALSE"));

        LLVMBuildCondBr(sa -> builder, pred, NEW_EQ, rlabel_blk);
    } else {
        assert(!strcmp(rlabel, "FALSE"));

        LLVMBuildCondBr(sa -> builder, pred, llabel_blk, NEW_EQ);
    }

    LLVMDeleteInstruction(cmp);
}

static void __insert(search_attr *sa, int32_t val) {
    LLVMBasicBlockRef leaf_node = get_end_node(sa, val);
    
    if (leaf_node == NULL)
        return;
    
    append_to_end_node(sa, val, leaf_node);
}

/*----------------------- API --------------------------------*/
static void insert(search_attr *sa, int32_t val) {
    __insert(sa, val);
}

static void clean_up(search_attr *sa) {
    // TODO: deallocate strings of basic block names

    LLVMDisposeBuilder(sa -> builder);
    LLVMDisposeExecutionEngine(sa -> engine);
}

static search_attr init_tree (int32_t val) {
    search_attr sa = create_module();
    verify_module(&sa);
    create_engine(&sa);
    __insert_initial(&sa, val);

    return sa;
}

// static bool has_val (search_attr *sa, int32_t val) {
//     #ifdef DEBUG_BST_IR
//     printf("Running has_val\n");
//     #endif
//     bool (*has_val)(int) = (bool (*)(int)) LLVMGetFunctionAddress(sa -> engine, "has_val");
//     return has_val(val);
// }

static bool (*get_has_val(search_attr *sa)) (int32_t) {
    #ifdef DEBUG_BST_IR
    printf("Getting has_val\n");
    #endif
    bool (*has_val)(int) = (bool (*)(int)) LLVMGetFunctionAddress(sa -> engine, "has_val");
    return has_val;
}

static void print(search_attr *sa, char *file_name) {
    output_bc(sa, file_name);
}

bst_ir init_bst_ir(void) {
    bst_ir bi = {
        .init_tree = init_tree,
        .insert = insert,
        .get_has_val = get_has_val,
        .clean_up = clean_up,
        .print = print
    };
    return bi;
} 