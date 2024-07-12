#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "rbtree.h"

static void rbtree_left_rotate(struct rbtree *tree, struct rbnode *p);
static void rbtree_right_rotate(struct rbtree *tree, struct rbnode *p);
static void rbtree_balance_after_insert(struct rbtree *tree, struct rbnode *p);
static void rbtree_print_inorder(const struct rbtree *tree, const struct rbnode *node);

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
    tree->root = tree->nil;

    return tree;
}

/* Free all nodes recursively*/
static void rbtree_free_nodes(struct rbtree *tree, struct rbnode *node){
    if (node == tree->nil)
        return;

    rbtree_free_nodes(tree, node->left);
    rbtree_free_nodes(tree, node->right);
    free(node);
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
    struct rbnode *temp = tree->root;

    while (temp != tree->nil && key != temp->key) {
        if (key < temp->key)
            temp = temp->left; 
        else 
            temp = temp->right;
    }
    
    if (temp != tree->nil)
        return temp;
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

/* Internal inorder traversal helper */
static void rbtree_print_inorder(const struct rbtree *tree, const struct rbnode *node) {
    if (node == tree->nil)
        return;
    rbtree_print_inorder(tree, node->left);
    printf("%d%c ", node->key, node->color == RB_RED ? 'R' : 'B');
    rbtree_print_inorder(tree, node->right);
}

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