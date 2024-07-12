#ifndef RBTREE_H
#define RBTREE_H

enum rb_color {
	RB_RED,
	RB_BLACK
};

struct rbnode {
	int key;
	enum rb_color color;
	struct rbnode *parent;
	struct rbnode *left;
	struct rbnode *right;
};

struct rbtree {
	struct rbnode *root;
	struct rbnode *nil;
};

/* API functions */
struct rbtree *rbtree_create(void);
void rbtree_destroy(struct rbtree *tree);

bool rbtree_insert(struct rbtree *tree, int key);
struct rbnode *rbtree_search(const struct rbtree *tree, int key);
bool rbtree_remove(struct rbtree *tree, int key);
void rbtree_print_elements(const struct rbtree *tree);

#endif /* _RBTREE_H */