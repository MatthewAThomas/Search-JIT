#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "bst_data.h"

// returns null if unsuccessful
static node *create_node(int32_t val) {
    node *new_node;

    if ((new_node = (node *) malloc(sizeof(node))) == NULL)
        return NULL;

    new_node -> val = val;
    new_node -> left = NULL;
    new_node -> right = NULL;

    return new_node;
}

// returns null if unsuccessful
static node *init_tree(int32_t val) {
    return create_node(val);
}

// returns true if val inserted successfully. Otherwise, false.
static bool insert(node *head, int32_t val) {
    while (true) {
        int32_t curr_val = head -> val;

        if (val < curr_val) {
            if (head -> left == NULL) {
                head -> left = create_node(val);
                return (head -> left != NULL); 
            } else {
                head = head -> left;
            }
        }

        if (val > curr_val) {
            if (head -> right == NULL) {
                head -> right = create_node(val);
                return (head -> right != NULL);
            } else {
                head = head -> right;
            }
        }

        if (val == curr_val)
            return true;
    }
}

static bool has_val(node *head, int32_t val) {
    while (head != NULL) {
        if (val < head -> val)
            head = head -> left;
        
        else if (val > head -> val)
            head = head -> right;
        
        else
            return true;
    }
    return false;
}

static void print_helper(node *head, int offset) {
    if (head == NULL)
        return;
    
    for (int i = 0; i <= offset; i++) {
        printf("  ");
    }
    printf("%d\n", head -> val);

    print_helper(head -> left, offset + 1);
    print_helper(head ->  right, offset + 1);
}

static void print(node *head) {
    print_helper(head, 0);
}


bst_data init_bst_data(void) {
    bst_data d;

    d.init_tree = init_tree;
    d.insert = insert;
    d.has_val = has_val;
    d.print = print;

    return d;
}