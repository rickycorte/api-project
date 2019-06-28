#include <stdlib.h>
#include <stdio.h>

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

#ifdef DEBUG

    void rb_depth_print(rbNode *node, int depth)
    {
        if(node == NULL && depth == 0) printf("- ");
        if(node == NULL) return;
        if(depth == 0)
        {
            printf("(%s%d) ",(node->color == RED)? "R" : "B", node->data);
        }
        else
        {
            rb_depth_print(node->left, depth -1);
            rb_depth_print(node->right, depth -1);
        }       
    }

    void rb_print(rbNode *root)
    {
        if(!root) 
        {
            printf("Empty\n");
            return;
        }

        int rb_height = 0;
        rbNode *node = root;
        // this should find the height
        while(node != NULL)
        {
            rb_height++;
            if(node->left)
                node = node->left;
            else if(node->right) node = node->right;
            else node = NULL;
        }
        printf("rb height (NOT CORRECT) %d\n", rb_height);
        for(int i = 0; i < rb_height +1; i++)
        {
            for(int j = 0; j < rb_height-i-1; j++)
                printf("     ");
            
            rb_depth_print(root, i);
            printf("\n");
        }
        printf("\n");
    }

#endif


inline static void rb_LeftRotate(rbNode **root, rbNode* x)
{
    rbNode *y = x->right;

    x->right = y->left;
    if(y->left)
        y->left->parent = x;
    y->parent = x->parent;
    if(x->parent == NULL)
        (*root) = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else 
        x->parent->right = y;
    
    y->left = x;
    x->parent = y;                   
}


inline static void rb_RightRotate(rbNode **root, rbNode* x)
{
    rbNode *y = x->left;

    x->left = y->right;
    if(y->right)
        y->right->parent = x;
    y->parent = x->parent;
    if(x->parent == NULL)
        (*root) = y;
    else if(x == x->parent->right)
        x->parent->right = y;
    else 
        x->parent->left = y;
    
    y->right = x;
    x->parent = y;    
}


inline static void rb_InsertFixup(rbNode **root, rbNode* z)
{
    rbNode *x, *y;
    while(z && z->parent && z->parent->color == RED)
    {
        if(z->parent == z->parent->parent->left)
        {
            y = z->parent->parent->right;
            if(y && y->color == RED)
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
            if(y && y->color == RED)
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
                        rb_RightRotate(root, z);
                    }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rb_LeftRotate(root, z->parent->parent);
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
void rb_Insert(rbNode **rb_root, int data)
{
    rbNode *new_node = (rbNode*)malloc(sizeof(rbNode));
    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    new_node->color = RED;

    // insert root if tree is empty
    if (*rb_root == NULL)
    {
        new_node->color = BLACK;
        (*rb_root) = new_node;
        return;
    }
    
    //bst insertion
    rbNode *y = NULL;
    rbNode *x = *rb_root;
    while(x != NULL)
    {
        y = x;
        if(data < x->data)
            x = x->left;
        else
            x = x->right;        
    }
    new_node->parent = y;

    if(data < y->data)
        y->left = new_node;
    else
        y->right = new_node;

    rb_InsertFixup(rb_root, new_node);
    printf("Inserted %d\n", data);
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
    rbNode *z = rb_Search(*rb_root, data);
    if(!z) return;

    rbNode *y;

    if(z->left == NULL || z->right == NULL)
    {

    }
}