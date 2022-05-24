#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "RBT.h"

//definition of Red Black Tree 
struct RedBlackTree
{int key; rblink left, right, parent; int clr;}; //red is 1, black is 0

//auxiliary function that creates a new node, initializes each struct variable with a certain value 
//and returns a pointer to the node
rblink CreateNode(int key, rblink left, rblink right, rblink parent, int clr)
{
    rblink new = malloc(sizeof (struct RedBlackTree));
    new->key=key;
    new->left=left;
    new->right=right;
    new->parent=parent;
    new->clr=clr;
    return new;
}

//auxiliary function that performs standard Binary Search Tree insertion
rblink BSTInsert(rblink root, rblink node)
{   
    if (root==NULL)
        return node;
    if (node->key<root->key) {      //if key to be inserted is less than current node's key
        rblink leftchild = BSTInsert(root->left, node);     //traverse the left subtree
        root->left = leftchild;             //make leftchild the left of root
        leftchild->parent = root;           //set leftchild's parent to root
    } else if (node->key>root->key) {   //if key to be inserted is greater than current node's key
        rblink rightchild = BSTInsert(root->right, node);       //traverse the right subtree
        root->right = rightchild;                   //make rightchild the right of root
        rightchild->parent = root;      //set rightchild's parent to root
    }

    return root;
}

//auxiliary function that performs left rotation
void LeftRotation(rblink **root, rblink node)
{
    rblink temp = node->right; //temp is right child of node
    node->right = temp->left; //make right child of node the left of temp

    if (node->right!=NULL) {        //if node has a right child, or equally if temp has a left child
        node->right->parent = node; //set its parent to node
    }
    temp->parent = node->parent;    //make the parent of temp, parent of node

    if (node->parent==NULL) {   //if root was given as node,
        **root=temp;            //make temp the root
    } else if (node==node->parent->right) {     //else if node is a right child
        node->parent->right=temp;       //make temp the right child of node's parent
    } else {                            //else
        node->parent->left=temp;        //make temp the left child of node's parent
    }
    temp->left=node; //make node the left child of temp
    node->parent=temp; //make temp the parent of node
}

//auxiliary function that performs right rotation
void RightRotation(rblink **root, rblink node)
{

    rblink temp = node->left; //make temp left child of node
    node->left = temp->right;   //make left child of  node the right child of temp

    if (node->left!=NULL) {         //if node has a left child, or equally if temp has a right child
        node->left->parent=node;    //set its parent to node
    }
    temp->parent = node->parent;    //make the parent of temp, parent of node
    
    if (node->parent==NULL) {           //if root was given as node,
        **root=temp;                    //make temp the root
    } else if (node==node->parent->left) {      //else if node is a left child
        node->parent->left=temp;            //make temp the left child of node's parent
    } else {                                //else
        node->parent->right=temp;           //make temp the right child of node's parent
    }
    temp->right=node;       //make node the right child of temp
    node->parent=temp;      //make temp the parent of node
}


//function to insert a new key into a Red Black Tree  
void InsertKey(rblink *root, int key)
{
    printf("Inserting item %d\n", key);
    rblink new = CreateNode(key, NULL, NULL, NULL, 1); //creating a new node using CreateNode()
    *root = BSTInsert(*root, new);          //inserting the node using BSTInsert()
    rblink uncle = NULL;
    
    //violations in the properties of a red black tree caused by the insertion will be fixed in the while loop
    while (new->parent!=NULL && new->parent->clr==1) {
        //CASE 1: parent of new node is left child of grandfather
        if (new->parent==new->parent->parent->left) {
            uncle = new->parent->parent->right;
            //CASE 1.1: if uncle of new node is red, recolor 
            if (uncle!=NULL && uncle->clr==1) {
                new->parent->parent->clr=1;
                uncle->clr=0;
                new->parent->clr=0;
                new=new->parent->parent;
            }
            else {
                //CASE 1.2: if uncle is black and new node is a right child, perform left rotation
                if (new==new->parent->right) {
                    new = new->parent;
                    LeftRotation(&root, new);
                }
                //CASE 1.3: if uncle is black and new node is a left child, recolor and perform right rotation 
                new->parent->clr=0;
                new->parent->parent->clr=1;
                RightRotation(&root, new->parent->parent);

            }
        } 
        //CASE 2: parent of new node is right child of grandfather (following cases are the sameas above, but inverted)
        else {
            uncle = new->parent->parent->left;
            //CASE 2.1: if uncle of new node is red, recolor
            if (uncle!=NULL && uncle->clr==1) {
            new->parent->parent->clr=1;
            uncle->clr=0;
            new->parent->clr=0;
            new=new->parent->parent;
            }
            else {
                //CASE 2.2 if uncle is black and new node is a left child, perform right rotation
                if (new==new->parent->left) {
                new = new->parent;
                RightRotation(&root, new);
                }
                //CASE 2.3 if uncle is black and new node is a right child, recolor and perform left rotation 
                new->parent->clr=0;
                new->parent->parent->clr=1;
                LeftRotation(&root, new->parent->parent);
            }
        }
    }
    (*root)->clr=0;    //root must always be black
}

//function to print the elements of the tree in ascending order
void PrintElements(rblink root)
{
    if (root!=NULL) {
        PrintElements(root->left);  //print left subtree
        printf("%d ", root->key);   //print root
        PrintElements(root->right); //print right subtree
    }
}

//function to search for a given key in the tree and return its node if found
rblink Search(rblink root, int key)
{
    //base cases
    if (root==NULL || root->key==key) { //root is null or key is found in the root
        return root;
    }
    if (key<root->key)      //if key is smaller than the root's key
        return Search(root->left, key);     //traverse the left subtree
    if (key>root->key)      //if key is greater than the root's key
        return Search(root->right, key);    //traverse the right subtree
}

//function to return the key of a given node, for information hiding purposes
int Data(rblink node)
{
    return (node->key);
}