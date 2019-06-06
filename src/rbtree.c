#include <stdlib.h>
#include<stdio.h>

enum RB_COLOR {RED, BLACK};

/**
*  Red-Black tree node structure 
*/
typedef struct s_rbNode
{
    int data;
    enum RB_COLOR color;

    struct s_rbNode *left, *right, *parent; 

} rbNode;


static void rb_LeftRotate(rbNode **root, rbNode* x)
{
    rbNode *y = x->right;

    if(y->left != NULL)
        y->left->parent = x;

    y->parent = x->parent;

    if(x->parent == NULL)
        (*root) = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;    
}

static void rb_RightRotate(rbNode **root, rbNode* x)
{
    rbNode *y = x->left;

    if(y->right != NULL)
        y->right->parent = x;

    y->parent = x->parent;

    if(x->parent == NULL)
        (*root) = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;

    y->right = x;
    x->parent = y;    
}


static void rb_InsertFixup(rbNode **root, rbNode* z)
{
    rbNode *x, *y;


}


/**
 * Inserisci un elemento nuovo nell'abero binario
 * @param rb_root radice dell'abero binario, puo puntare a null se l'abero va creato
 * @param data dati da inserire nel nodo
 */
void rb_Insert(rbNode **rb_root, int data)
{
    rbNode *z = (struct node*)malloc(sizeof(rbNode));
    z->data = data;
    z->left = NULL;
    z->right = NULL;
    z->parent = NULL;

    if (*rb_root == NULL)
    {
        z->color = BLACK;
        (*rb_root) = z;
        return;
    }
    
    rbNode *y = NULL;
    rbNode *x = *rb_root;

    while(y != NULL)
    {
        y = x;
        if(z->data < x->data)
            x = x->left;
        else
            x = x->right;        
    }
    z->parent = y;
    if(z->data < y->data)
        y->left = z;
    else
        y->right = z;
    
    z->left = NULL;
    z->right = NULL;
    z->color = RED;

    rb_InsertFixup(rb_root, z);
}