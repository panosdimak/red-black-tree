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

/* Rotate left around node */
static void rbtree_left_rotate(struct rbtree *tree, struct rbnode *node) {
    struct rbnode *pivot = node->right;
    node->right = pivot->left;

    if (pivot->left != tree->nil)
        pivot->left->parent = node;

    pivot->parent = node->parent;

    if (node->parent == tree->nil)
        tree->root = pivot;
    else if (node == node->parent->left)
        node->parent->left = pivot;
    else
        node->parent->right = pivot;

    pivot->left = node;
    node->parent = pivot;
}

/* Rotate right around node */
static void rbtree_right_rotate(struct rbtree *tree, struct rbnode *node) {
    struct rbnode *pivot = node->left;
    node->left = pivot->right;

    if (pivot->right != tree->nil)
        pivot->right->parent = node;

    pivot->parent = node->parent;

    if (node->parent == tree->nil)
        tree->root = pivot;
    else if (node == node->parent->right)
        node->parent->right = pivot;
    else
        node->parent->left = pivot;

    pivot->right = node;
    node->parent = pivot;
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

static void rbtree_balance_after_delete(struct rbtree *tree, struct rbnode *node) {
    while (node != tree->root && node->color == RB_BLACK) {
        if (node == node->parent->left) {
            struct rbnode *sibling_node = node->parent->right;
            
            if (sibling_node == tree->nil) {
                node = node->parent;
                continue;
            }
            
            if (sibling_node->color == RB_RED) {
                /* Case 1 */
                sibling_node->color = RB_BLACK;
                node->parent->color = RB_RED;
                rbtree_left_rotate(tree, node->parent);
                sibling_node = node->parent->right;
            } 
            
            if (sibling_node->left->color == RB_BLACK && 
                sibling_node->right->color ==  RB_BLACK) {
                /* Case 2 */
                sibling_node->color = RB_RED;
                node = node->parent;
            } else {
                if (sibling_node->right->color == RB_BLACK) {
                    /* Case 3 */
                    sibling_node->left->color = RB_BLACK;
                    sibling_node->color = RB_RED;
                    rbtree_right_rotate(tree, sibling_node);
                    sibling_node = node->parent->right;
                }

                /* Case 4 */
                sibling_node->color = node->parent->color;
                node->parent->color = RB_BLACK;
                sibling_node->right->color = RB_BLACK;
                rbtree_left_rotate(tree, node->parent);
                node = tree->root;
            }
        } else {
            struct rbnode *sibling_node = node->parent->left;

            if (sibling_node == tree->nil) {
                node = node->parent;
                continue;
            }

            /* Case 1 */
            if (sibling_node->color == RB_RED) {
                sibling_node->color = RB_BLACK;
                node->parent->color = RB_RED;
                rbtree_right_rotate(tree, node->parent);
                sibling_node = node->parent->left;
            }

            if (sibling_node->right->color == RB_BLACK && 
                sibling_node->left->color == RB_BLACK) {
                /* Case 2 */
                sibling_node->color = RB_RED;
                node = node->parent;
            } else {
                if (sibling_node->left->color == RB_BLACK) {
                    /* Case 3 */
                    sibling_node->right->color = RB_BLACK;
                    sibling_node->color = RB_RED;
                    rbtree_left_rotate(tree, sibling_node);
                    sibling_node = node->parent->left;
                }

                /* Case 4 */
                sibling_node->color = node->parent->color;
                node->parent->color = RB_BLACK;
                sibling_node->left->color = RB_BLACK;
                rbtree_right_rotate(tree, node->parent);
                node = tree->root;
            }
        }
    }
    node->color = RB_BLACK;
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

static void rbtree_free_nodes(struct rbtree *tree, struct rbnode *root) {
    if (root == tree->nil)
        return;

    struct node_stack *stack_a = stack_create(64);
    struct node_stack *stack_b = stack_create(64);
    if (!stack_a || !stack_b) {
        stack_destroy(stack_a);
        stack_destroy(stack_b);
        return;
    }

    if (!stack_push(stack_a, root)) {
        if (stack_a) stack_destroy(stack_a);
        if (stack_b) stack_destroy(stack_b);
        return;
    }

    while (!stack_empty(stack_a)) {
        struct rbnode *node = stack_pop(stack_a);
        if (!stack_push(stack_b, node)) { 
            if (stack_a) stack_destroy(stack_a);
            if (stack_b) stack_destroy(stack_b);
            return;
        }

        if (node->left != tree->nil)
            if (!stack_push(stack_a, node->left)) {
                if (stack_a) stack_destroy(stack_a);
                if (stack_b) stack_destroy(stack_b);
                return;
            }
        if (node->right != tree->nil)
            if (!stack_push(stack_a, node->right)) {
                if (stack_a) stack_destroy(stack_a);
                if (stack_b) stack_destroy(stack_b);
                return;
            }
    }

    while (!stack_empty(stack_b)) {
        struct rbnode *node = stack_pop(stack_b);
        free(node);
    }

    stack_destroy(stack_a);
    stack_destroy(stack_b);
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

static struct rbnode *rbtree_minimum(const struct rbtree *tree, struct rbnode *node) {
    while (node->left != tree->nil)
        node = node->left;
    return node;
}

static void rbtree_transplant(struct rbtree *tree, struct rbnode *target, struct rbnode *child) {
    if (target->parent == tree->nil) 
        tree->root = child;
    else if (target == target->parent->left) 
        target->parent->left = child;
    else 
        target->parent->right = child;
        
    child->parent = target->parent;
}

bool rbtree_delete(struct rbtree *tree, int key) {
    struct rbnode *node = rbtree_search(tree, key);
    if (node == NULL) 
        return false;

    struct rbnode *replacement_node = node;
    enum rb_color replacement_original_color = replacement_node->color;
    struct rbnode *fixup_node;

    if (node->left == tree->nil) {
        fixup_node = node->right;
        rbtree_transplant(tree, node, node->right);
    } else if (node->right == tree->nil) {
        fixup_node = node->left;
        rbtree_transplant(tree, node, node->left);
    } else {
        replacement_node = rbtree_minimum(tree, node->right);
        replacement_original_color = replacement_node->color;
        fixup_node = replacement_node->right;

        if (replacement_node->parent == node) {
            fixup_node->parent = replacement_node;
        } else {
            rbtree_transplant(tree, replacement_node, replacement_node->right);
            replacement_node->right = node->right;
            if (replacement_node->right != tree->nil)
                replacement_node->right->parent = replacement_node;            
        }

        rbtree_transplant(tree, node, replacement_node);
        replacement_node->left = node->left;
        if (replacement_node->left != tree->nil)
            replacement_node->left->parent = replacement_node;
        replacement_node->color = node->color;
    }

    free(node);

    if (replacement_original_color == RB_BLACK)
        rbtree_balance_after_delete(tree, fixup_node);

    return true;
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