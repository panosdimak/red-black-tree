typedef struct RedBlackTree* rblink;

void InsertKey(rblink *root, int key);
void PrintElements(rblink root);
rblink Search(rblink root, int key);
int Data(rblink node);