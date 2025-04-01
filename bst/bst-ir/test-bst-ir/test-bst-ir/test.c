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

#include "../../bst_ir.h"

void test_1_node(void) {
    printf("Running test: test 1 node\n");
    
    bst_ir bi = init_bst_ir();
    search_attr sa = bi.init_tree(0);
    bi.print(&sa);

    for (int32_t val = -1; val < 2; val++) {
        printf("has_val(%d) := %d\n", val, bi.has_val(&sa, val));
    }

    bi.clean_up(&sa);

    printf("\n");
}

void test_append_1_node_1(void) {
    printf("Running test: test append 1 node (1)");

    bst_ir bi = init_bst_ir();
    search_attr sa = bi.init_tree(0);
    bi.print(&sa);

    int32_t val;
    printf("Enter number to be inserted\n");
    scanf("%d", &val);

    bi.insert(&sa, val);

    bi.print(&sa);

    bi.clean_up(&sa);

    printf("\n");
}

void test_append_2_node_1(void) {};

int main(int argc, char **argv) {
    // test_1_node();

    test_append_1_node_1();
    
    return 0;
}