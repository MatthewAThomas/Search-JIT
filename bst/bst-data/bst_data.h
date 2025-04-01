#ifndef BST_DATA_H
#define BST_DATA_H

#include <stdint.h>
#include <stdbool.h>

typedef struct node {
    int32_t val;
    struct node *left;
    struct node *right;
} node;

typedef struct bst_data {
    node *(*init_tree) (int32_t);
    bool (*insert) (node *, int32_t);
    bool (*has_val) (node *, int32_t);
    void (*print) (node *);
} bst_data;

bst_data init_bst_data(void);

#endif // BST_DATA_H