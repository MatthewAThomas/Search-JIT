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
    bi.print(&sa, (char *) "has_val.bc");

    bool (*has_val) (int32_t) = bi.get_has_val(&sa);

    for (int32_t val = -1; val < 2; val++) {
        printf("has_val(%d) := %d\n", val, has_val(val));
    }

    bi.clean_up(&sa);

    printf("\n");
}

void test_append_1_node_1(void) {
    printf("Running test: test append 1 node (1)\n");

    bst_ir bi = init_bst_ir();
    search_attr sa = bi.init_tree(0);
    bi.print(&sa, (char *) "first.bc");

    int32_t val;
    printf("Enter number to be inserted\n");
    scanf("%d", &val);

    bi.insert(&sa, val);

    bi.print(&sa, (char *) "second.bc");

    bi.clean_up(&sa);

    printf("\n");
}

void test_append_2_node_1(void) {
    printf("Running test: test append 2 node (1)\n");

    bst_ir bi = init_bst_ir();
    search_attr sa = bi.init_tree(0);
    bi.print(&sa, (char *) "first.bc");

    bi.insert(&sa, 1);
    bi.print(&sa, (char *) "second.bc");

    bool (*has_val) (int32_t) = bi.get_has_val(&sa);
    has_val(2);

    bi.insert(&sa, -1);
    bi.print(&sa, (char *) "third.bc");

    bi.clean_up(&sa);

    printf("\n");
}

void test_append_arb_node_1(void) {
    bst_ir bi = init_bst_ir();
    search_attr sa = bi.init_tree(0);

    // bi.insert(&sa, 1);
    // bi.has_val(&sa, 2);
    // bi.has_val(&sa, 3);
    // printf("1\n");
    // bi.insert(&sa, 0);
    // bi.has_val(&sa, 3);
    // printf("2\n");
    // bi.insert(&sa, 5);
    // bi.has_val(&sa, 4);
    // printf("3\n");
    // bi.insert(&sa, 8);
    // bi.has_val(&sa, 1);
    // printf("4\n");
    // bi.insert(&sa, 2);
    // bi.has_val(&sa, 4);

    // bi.insert(&sa, 1);
    // bi.insert(&sa, 2);
    // bi.insert(&sa, 3);
    // bi.insert(&sa, 4);
    // bi.has_val(&sa, 0);
    // bi.insert(&sa, 5);

    // bi.has_val(&sa, 0);
    // bi.insert(&sa, 1);
    // bi.insert(&sa, 2);
    // bi.insert(&sa, 3);
    // bi.insert(&sa, 4);
    // bi.insert(&sa, 5);


    bi.clean_up(&sa);
}

#define GET_NUM(S, SIZE, VAL) \
    ((fgets(S, SIZE, stdin) != NULL) && (S[0] != '\n') \
    && (sscanf(S, "%d", &VAL)))

// void test_has_val_1(void) {
//     printf("Running test: test has_val (1)\n");

//     bst_ir bi = init_bst_ir();

//     int32_t val;
//     printf("Enter number for initial tree node\n");
//     scanf("%d", &val);
//     while (getchar() != '\n'); // clear input buffer
//     search_attr sa = bi.init_tree(val);
//     bi.print(&sa, (char *) "has_val.bc");

//     char number[10];
//     printf("Enter number to insert\n");
//     for (int i = 1; (i < 100) && GET_NUM(number, 10, val); i++) {
//         bi.insert(&sa, val);
//         bi.print(&sa, (char *) "has_val.bc");
        
//         printf("Enter number to see if tree contains it:");
//         while (GET_NUM(number, 10, val)) {
//             printf("has_val(%d) := %d\n", val, bi.has_val(val));
//             printf("Enter number to see if tree contains it:\n");
//         }
        
//         printf("Enter number to insert\n");
//     }

//     bi.clean_up(&sa);
//     printf("\n");
// }


void test_has_val_2(void) {
    printf("Running test: test has_val (1)\n");

    #define max_num_str_len 13

    bst_ir bi = init_bst_ir();

    int32_t val;
    printf("Enter number for initial tree node\n");
    scanf("%d", &val);
    while (getchar() != '\n'); // clear input buffer
    search_attr sa = bi.init_tree(val);
    bi.print(&sa, (char *) "has_val.bc");

    char number[max_num_str_len];
    printf("Enter number to insert\n");
    for (int i = 1; (i < 100) && GET_NUM(number, max_num_str_len, val); i++) {
        bi.insert(&sa, val);
        bi.print(&sa, (char *) "has_val.bc");
        printf("Enter number to insert\n");
    }

    bool (*has_val) (int32_t) = bi.get_has_val(&sa);
    printf("Enter number to see if tree contains it:");
    while (GET_NUM(number, max_num_str_len, val)) {
        printf("has_val(%d) := %d\n", val, has_val(val));
        printf("Enter number to see if tree contains it:\n");
    }

    bi.clean_up(&sa);
    printf("\n");
}

int main(int argc, char **argv) {
    // test_1_node();
    //test_append_1_node_1();
    // test_append_2_node_1();
    test_has_val_2();
    
    return 0;
}