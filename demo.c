#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "rbtree.h"
#define N 10
#define LIMIT 51

int main()
{
    srand(time(0));
    struct rbtree *root = rbtree_create();
    for (int i = 0; i < N; i++) {
        int x =  rand() % LIMIT;
        rbtree_insert(root, x);
        printf("Inserting item \"%d\"...\n", x);
    }

    printf("\nPrinting items in ascending order...\n");
    rbtree_print_elements(root);
    printf("\n");

    int x = rand() % LIMIT;
    printf("Searching for item \"%d\"... ", x);
    struct rbnode *s = rbtree_search(root, x);
    if (s)
        printf("Item %d is in the tree\n", s->key);
    else 
        printf("Item %d not found\n", x);

    rbtree_destroy(root);
    return 0;
}