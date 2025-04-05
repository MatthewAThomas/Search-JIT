#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#include "bst-data/bst_data.h"
#include "bst-ir/bst_ir.h"
#include "benchmark-utils/data.h"
#include "benchmark-utils/queries.h"

typedef struct {
    bst_data bd;
    node *head;
} bst_data_bundle;

typedef struct {
    bst_ir bi;
    search_attr sa;
} bst_ir_bundle;

typedef struct  {
    bst_data_bundle db;
    bst_ir_bundle irb;
} bsts;


/*------------------------Insertion------------------------------*/

static bst_data_bundle insert_bst_data(int n) {
    clock_t begin = clock();

    bst_data _data = init_bst_data();
    node *head = _data.init_tree(3);
    
    for (int i = 0; i < n; i++) {
        _data.insert(head, vals[i]);
    }

    clock_t end = clock();
    double duration = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%d elements inserted into data bst in %f seconds\n", n, duration);
    
    bst_data_bundle bst_db = {.bd = _data, .head = head};
    return bst_db;
}

static bst_ir_bundle insert_bst_ir(int n) {
    clock_t begin = clock();

    bst_ir bi = init_bst_ir();
    search_attr sa = bi.init_tree(0);

    for (int i = 0; i < n; i++) {
        bi.insert(&sa, vals[i]);
    }

    clock_t end = clock();
    double duration = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%d elements inserted into JITed bst in %f seconds\n", n, duration);

    bst_ir_bundle bst_irb = {.bi = bi, .sa = sa};
    return bst_irb;
}

static bsts benchmark_insert(int n) {
    bsts b;

    // consider playing around with the order that their called
    // available memory resources may play a role in performance (idk though)
    b.irb = insert_bst_ir(n);
    b.db = insert_bst_data(n);

    return b;
}


/*------------------------Querying---------------------------*/
void clean_up_bst_ir(bsts *b) {}

void query_bst_data(int n, bst_data_bundle *db, bool *check_array) {
    clock_t begin = clock();
    
    for (int i = 0; i < n; i++) {
        check_array[i] = (db -> bd).has_val(db -> head, queries[i]);
    }

    clock_t end = clock();
    double duration = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%d elements queries performed on data bst in %f seconds\n", n, duration);
}

void query_bst_ir(int n, bst_ir_bundle *irb, bool *check_array) {
    clock_t begin = clock();
    
    bool (*has_val) (search_attr *, int32_t) = (irb -> bi).has_val;
    search_attr sa = (irb -> sa);
    for (int i = 0; i < n; i++) {
        check_array[i] = has_val(&sa, queries[i]);
    }

    clock_t end = clock();
    double duration = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%d elements queries performed on JITed bst in %f seconds\n", n, duration);
}

static void verify(int n, bool *ca_data, bool *ca_ir) {
    for (int i = 0; i < n; i++) {
        if (ca_data[i] != ca_ir[i]) {
            printf("Data and IR BST queries don't match!\n");
            exit(-1);
        }
    }
}

void benchmark_query(bsts b, int n) {
    bool *data_check_array, *ir_check_array;

    if (!(data_check_array = (bool *) malloc(n * sizeof(bool)))) {
        printf("Ran out of memory verifying data query\n");
        exit(-1);
    }

    if (!(ir_check_array = (bool *) malloc(n * sizeof(bool)))) {
        printf("Ran out of memory verifying ir query\n");
        exit(-1);
    }

    // again, consider playing around with the order in which these are called
    query_bst_data(n, &(b.db), data_check_array);
    query_bst_ir(n, &(b.irb), ir_check_array);

    verify(n, data_check_array, ir_check_array);
}

int main(int argc, char **argv) {
    bsts b = benchmark_insert(NUM_INSERTIONS);
    benchmark_query(b, NUM_QUERIES);
    return 0;
}