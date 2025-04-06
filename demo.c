#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "rbtree.h"

int main(void) {
    struct rbtree *tree = rbtree_create();
    if (!tree) {
        fprintf(stderr, "Error: failed to initialize tree.\n");
        exit(1);
    }

    int values[] = {20, 15, 30, 10, 18, 25, 40, 22};
    size_t n = sizeof(values)/sizeof(values[0]);
    
    printf("=== Inserting elements ===\n");
    for (size_t i = 0; i < n; i++) {
        printf("Insert %d\n", values[i]);
        rbtree_insert(tree, values[i]);
    }

    printf("\nTree after insertions:\n");
    rbtree_print_elements(tree);

    printf("\n=== Searching ===\n");
    int search_keys[] = {18, 99};
    for (size_t i = 0; i < 2; i++) {
        struct rbnode *res = rbtree_search(tree, search_keys[i]);
        if (res)
            printf("Found key %d (color: %s)\n", res->key, res->color == RB_BLACK ? "BLACK" : "RED");
        else
            printf("Key %d not found\n", values[i]);
    }

    printf("\n=== Deleting elements ===\n");
    int delete_keys[] = {10, 15, 20, 25};
    size_t m = sizeof(delete_keys)/sizeof(delete_keys[0]);
    for (size_t i = 0; i < m; i++) {
        printf("Delete %d\n", delete_keys[i]);
        rbtree_delete(tree, delete_keys[i]);
        rbtree_print_elements(tree);
    }

    printf("\nFinal tree:\n");
    rbtree_print_elements(tree);

    rbtree_destroy(tree);
    return 0;
}