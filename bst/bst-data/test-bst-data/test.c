#include <stdio.h>
#include <assert.h>
#include "../bst_data.h"


void single(bst_data b) {
    printf("Test: single\n");
    node *head = b.init_tree(1);
    b.print(head);
    printf("\n");
}

void two_children_1(bst_data b) {
    printf("Test: two children 1\n");

    node *head = b.init_tree(2);
    b.insert(head, 1);
    b.insert(head, 3);

    b.print(head);

    printf("\n");
}

void two_children_2(bst_data b) {
    printf("Test: two children 2\n");

    node *head = b.init_tree(1);
    b.insert(head, 2);
    b.insert(head, 3);

    b.print(head);

    printf("\n");
}

void two_children_3(bst_data b) {
    printf("Test: two children 3\n");

    node *head = b.init_tree(3);
    b.insert(head, 2);
    b.insert(head, 1);

    b.print(head);

    printf("\n");
}

void more_children_1(bst_data b) {
    printf("Test: more children 1\n");
    
    node *head = b.init_tree(4);
    b.insert(head, 2);
    b.insert(head, 3);
    b.insert(head, 5);
    b.insert(head, 1);

    b.print(head);

    printf("\n");
}

void more_children_2(bst_data b) {
    printf("Test: more children 2\n");
    
    node *head = b.init_tree(5);
    b.insert(head, 7);
    b.insert(head, 3);
    b.insert(head, 6);
    b.insert(head, 8);
    b.insert(head, 4);
    b.insert(head, 2);
    b.insert(head, 1);

    b.print(head);

    printf("\n");
}


int main(int argc, char **argv) {

    bst_data b = init_bst_data();

    // single(b);
    // two_children_1(b);
    // two_children_2(b);
    // two_children_3(b);
    // more_children_1(b);
    more_children_2(b);

    return 0;
}