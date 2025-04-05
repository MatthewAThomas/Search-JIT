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


    b.has_val(head, -1);
    
    printf("has_val(%d) := %d\n", -1,b.has_val(head, -1));
    printf("has_val(%d) := %d\n", 0,b.has_val(head, 0));
    printf("has_val(%d) := %d\n", 1,b.has_val(head, 1));
    printf("has_val(%d) := %d\n", 2,b.has_val(head, 2));
    printf("has_val(%d) := %d\n", 3,b.has_val(head, 3));
    printf("has_val(%d) := %d\n", 4,b.has_val(head, 4));
    printf("has_val(%d) := %d\n", 5,b.has_val(head, 5));
    printf("has_val(%d) := %d\n", 6,b.has_val(head, 6));
    printf("has_val(%d) := %d\n", 7,b.has_val(head, 7));
    printf("has_val(%d) := %d\n", 8,b.has_val(head, 8));
    printf("has_val(%d) := %d\n", 9,b.has_val(head, 9));

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