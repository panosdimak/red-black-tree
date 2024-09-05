#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rbtree.h"

/* --- Stack Implementation for iterative tree traversal --- */

struct node_stack {
    struct rbnode **data;   /* array of pointers */
    size_t top;             /* next free slot (stack size) */
    size_t capacity;        /* current allocated size */
};

static struct node_stack *stack_create(size_t initial_capacity) {
    struct node_stack *s = malloc(sizeof(*s));
    if (!s)
        return NULL;

    s->data = malloc(initial_capacity * sizeof(*s->data));
    if (!s->data) {
        free(s);
        return NULL;
    };

    s->top = 0;
    s->capacity = initial_capacity;
    return s;
}

static bool stack_push(struct node_stack *s, struct rbnode *node) {
    if (s->top == s->capacity) {
        size_t new_capacity = s->capacity * 2;
        struct rbnode **tmp = realloc(s->data, new_capacity * sizeof(*tmp));
        if (!tmp)
            return false;
        
        s->data = tmp;
        s->capacity = new_capacity;
    }

    s->data[s->top++] = node;
    return true;
}

static inline struct rbnode *stack_pop(struct node_stack *s) {
    if (s->top == 0) {
        return NULL;
    }
    return s->data[--s->top];
}

static inline bool stack_empty(const struct node_stack *s) {
    return s->top == 0;
}

static void stack_destroy(struct node_stack *s) {
    free(s->data);
    free(s);
}

/* --- Tree Rotations --- */

/* Rotate left around node p */
static void rbtree_left_rotate(struct rbtree *tree, struct rbnode *p) {
    struct rbnode *c = p->right;
    p->right = c->left;

    if (c->left != tree->nil)
        c->left->parent = p;

    c->parent = p->parent;

    if (p->parent == tree->nil)
        tree->root = c;
    else if (p == p->parent->left)
        p->parent->left = c;
    else
        p->parent->right = c;

    c->left = p;
    p->parent = c;
}

/* Rotate right around node p */
static void rbtree_right_rotate(struct rbtree *tree, struct rbnode *p) {
    struct rbnode *c = p->left;
    p->left = c->right;

    if (c->right != tree->nil)
        c->right->parent = p;

    c->parent = p->parent;

    if (p->parent == tree->nil)
        tree->root = c;
    else if (p == p->parent->right)
        p->parent->right = c;
    else
        p->parent->left = c;

    c->right = p;
    p->parent = c;
}

/* --- Balancing operations --- */

static void rbtree_balance_after_insert(struct rbtree *tree, struct rbnode *new_node) {
    while (new_node->parent->color == RB_RED) {
        if (new_node->parent == new_node->parent->parent->left) {
            struct rbnode *uncle_node = new_node->parent->parent->right;
            if (uncle_node->color == RB_RED) {
                new_node->parent->color = RB_BLACK;
                uncle_node->color = RB_BLACK;
                new_node->parent->parent->color = RB_RED;
                new_node = new_node->parent->parent;
            } else {
                if (new_node == new_node->parent->right) {
                    new_node = new_node->parent;
                    rbtree_left_rotate(tree, new_node);
                }
                new_node->parent->color = RB_BLACK;
                new_node->parent->parent->color = RB_RED;
                rbtree_right_rotate(tree, new_node->parent->parent);
            }
        } else {
            struct rbnode *uncle_node = new_node->parent->parent->left;
            if (uncle_node->color == RB_RED) {
                new_node->parent->color = RB_BLACK;
                uncle_node->color = RB_BLACK;
                new_node->parent->parent->color = RB_RED;
                new_node = new_node->parent->parent;
            } else {
                if (new_node == new_node->parent->left) {
                    new_node = new_node->parent;
                    rbtree_right_rotate(tree, new_node);
                }
                new_node->parent->color = RB_BLACK;
                new_node->parent->parent->color = RB_RED;
                rbtree_left_rotate(tree, new_node->parent->parent);
            }
        }
    }
    tree->root->color = RB_BLACK;
}

/* --- Tree Traversal Helpers --- */
static void rbtree_print_inorder(const struct rbtree *tree, const struct rbnode *node) {
    if (node == tree->nil)
        return;
    rbtree_print_inorder(tree, node->left);
    printf("%d%c ", node->key, node->color == RB_RED ? 'R' : 'B');
    rbtree_print_inorder(tree, node->right);
}

/* --- Public API functions --- */
struct rbtree *rbtree_create(void) {
    struct rbtree *tree = malloc(sizeof(*tree));
    if (!tree)
        return NULL;

    tree->nil = malloc(sizeof(struct rbnode));
    if (!tree->nil) {
        free(tree);
        return NULL;
    }

    tree->nil->color = RB_BLACK;
    tree->nil->left = tree->nil;
    tree->nil->right = tree->nil;
    tree->nil->parent = tree->nil;
    tree->root = tree->nil;

    return tree;
}

static void rbtree_free_nodes (struct rbtree *tree, struct rbnode *root) {
    if (root == tree->nil)
        return;

    struct node_stack *A = stack_create(64);
    struct node_stack *B = stack_create(64);
    if (!A || !B) {
        stack_destroy(A);
        stack_destroy(B);
        return;
    }

    if (!stack_push(A, root)) {
        if (A) stack_destroy(A);
        if (B) stack_destroy(B);
        return;
    }

    while (!stack_empty(A)) {
        struct rbnode *node = stack_pop(A);
        if (!stack_push(B, node)) { 
            if (A) stack_destroy(A);
            if (B) stack_destroy(B);
            return;
        }

        if (node->left != tree->nil)
            if (!stack_push(A, node->left)) {
                if (A) stack_destroy(A);
                if (B) stack_destroy(B);
                return;
            }
        if (node->right != tree->nil)
            if (!stack_push(A, node->right)) {
                if (A) stack_destroy(A);
                if (B) stack_destroy(B);
                return;
            }
    }

    while (!stack_empty(B)) {
        struct rbnode *node = stack_pop(B);
        free(node);
    }

    stack_destroy(A);
    stack_destroy(B);
}

void rbtree_destroy(struct rbtree *tree) {
    if (!tree)
        return;

    rbtree_free_nodes(tree, tree->root);
    free(tree->nil);
    free(tree);
}

bool rbtree_insert(struct rbtree *tree, int key) {
    struct rbnode *new_node = malloc(sizeof(*new_node));
    if (!new_node)
        return false;

    new_node->key = key;
    new_node->color = RB_RED;
    new_node->left = tree->nil;
    new_node->right = tree->nil;

    struct rbnode *parent_node = tree->nil;
    struct rbnode *current_node = tree->root;

    while (current_node != tree->nil) {
        parent_node = current_node;
        if (new_node->key < current_node->key)
            current_node = current_node->left;
        else
            current_node = current_node->right;
    }

    new_node->parent = parent_node;
    if (parent_node == tree->nil)
        tree->root = new_node;
    else if (new_node->key < parent_node->key)
        parent_node->left = new_node;
    else
        parent_node->right = new_node;

    rbtree_balance_after_insert(tree, new_node);
    return true;
}

struct rbnode *rbtree_search(const struct rbtree *tree, int key) {
    struct rbnode *tmp = tree->root;

    while (tmp != tree->nil && key != tmp->key) {
        if (key < tmp->key)
            tmp = tmp->left; 
        else 
            tmp = tmp->right;
    }
    
    if (tmp != tree->nil)
        return tmp;
    return NULL;
}

/* TODO: implement rbtree_remove()*/
bool rbtree_remove(struct rbtree *tree, int key) {
    return false;
}

void rbtree_print_elements(const struct rbtree *tree) {
    if (!tree)
        return;
    if (tree->root == tree->nil) {
        printf("(empty)\n");
        return;
    }
    rbtree_print_inorder(tree, tree->root);
    printf("\n");
}