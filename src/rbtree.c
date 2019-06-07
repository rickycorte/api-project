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


inline static void rb_LeftRotate(rbNode **root, rbNode* x)
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


inline static void rb_RightRotate(rbNode **root, rbNode* x)
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


inline static void rb_InsertFixup(rbNode **root, rbNode* z)
{
    rbNode *x, *y;
    while(z->parent->color == RED)
    {
        if(z->parent == z->parent->parent->left)
        {
            y = z->parent->parent->right;
            if(y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if(z == z->parent->right)
                {
                    z = z->parent;
                    rb_LeftRotate(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rb_RightRotate(root, z->parent->parent);
            }
            
        }
        else
        {
            y = z->parent->parent->left;
            if(y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if(z == z->parent->left)
                    {
                        z = z->parent;
                        rb_LeftRotate(root, z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rb_RightRotate(root, z->parent->parent);
            }
        }      
    }

    (*root)->color = BLACK;
}


/**
 * Inserisci un elemento nuovo nell'abero binario
 * @param rb_root radice dell'abero binario, puo puntare a null se l'abero va creato
 * @param data dati da inserire nel nodo
 */
inline void rb_Insert(rbNode **rb_root, int data)
{
    rbNode *new_node = (struct node*)malloc(sizeof(rbNode));
    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    new_node->color = RED;

    if (*rb_root == NULL)
    {
        new_node ->color = BLACK;
        (*rb_root) = new_node;
        return;
    }
    
    //bst insertion
    rbNode *y = NULL;
    rbNode *x = *rb_root;
    while(x != NULL)
    {
        y = x;
        if(new_node ->data < x->data)
            x = x->left;
        else
            x = x->right;        
    }
    new_node ->parent = y;

    if(new_node->data < y->data)
        y->left = new_node;
    else
        y->right = new_node;

    rb_InsertFixup(rb_root, new_node);
}


/**
 *  Cerca un elemento nell'albero binario
 */
inline rbNode *rb_Search(rbNode *rb_root, int val)
{
    rbNode *itr = rb_root;
    while(itr != NULL && val != itr->data)
    {
        if(val < itr->data)
            itr = itr->left;
        else
            itr = itr->right;
    }

    return itr;
}


inline static rbNode *rb_TreeSuccessor(rbNode* x)
{
    rbNode *y;

    if(x->right != NULL)
    {
        while(x->right != NULL)
            x = x->right;
    }
    y= x->parent;
    while (y != NULL && x == y->right)
    {
        x = y;
        y->parent = y;
    }
    
    return y;
    
}

inline void rb_Delete(rbNode **rb_root, int data)
{
    rbNode *z = rb_Search(rb_root, data);
    if(!z) return 0;

    rbNode *y;

    if(z->left == NULL || z->right == NULL)
    {

    }
}