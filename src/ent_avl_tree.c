//
// Created by ricky on 17/07/19.
//

#include <stdlib.h>
#include <string.h>


typedef signed char result;

/**
 * Avl tree node
 */
typedef struct s_avlnode
{
    char *data;
    int relations;
    signed char balance;
    struct s_avlnode *parent, *left, *right;

} AvlNode;

/**
 * Avl tree container
 */
typedef struct
{
    AvlNode *root;

} AvlTree;

// sentinel node
// root is never a sentinel!
static AvlNode avl_sentinel = {0, 0, 0, NULL, &avl_sentinel, &avl_sentinel };

#define AVL_NIL &avl_sentinel


AvlTree *avl_init()
{
    AvlTree *t = malloc(sizeof(AvlTree));
    t->root = NULL;

    return  t;
}


static inline AvlNode* avl_leftRotation(AvlTree *tree, AvlNode *x)
{
    AvlNode *y = x->right;
    x->right = y->left;

    if(y->left != AVL_NIL)
        y->left->parent = x;
    if(y != AVL_NIL)
        y->parent = x->parent;

    if(x->parent)
    {
        if(x== x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    }
    else
    {
        tree->root = y;
    }

    y->left = x;
    if(x != AVL_NIL)
        x->parent = y;

    return y;
}


static inline AvlNode *avl_rightRotation(AvlTree *tree, AvlNode *x)
{
    AvlNode *y = x->left;
    x->left = y->right;

    if(y->right != AVL_NIL)
        y->right->parent = x;
    if(y != AVL_NIL)
        y->parent = x->parent;

    if(x->parent)
    {
        if(x== x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    }
    else
    {
        tree->root = y;
    }

    y->right = x;
    if(x != AVL_NIL)
        x->parent = y;

    return y;

}



AvlNode *avl_insert(AvlTree *tree, char *data, result *inserted)
{

    int cmp = 0;

    AvlNode *parent = NULL, *itr = tree->root;

    while(itr && itr != AVL_NIL)
    {
        cmp = strcmp(data, itr->data);

        if(cmp == 0)
        {
            //duplicate
            *inserted = 0;
            return  itr;
        }

        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;

    }

    //allocate
    AvlNode* node = malloc(sizeof(AvlNode));
    node->data = data;
    node->balance = 0;
    node->parent = parent;
    node->right = node->left = AVL_NIL;
    node->relations = 0;

    if(parent)
    {
        if(cmp > 0)
            parent->right = node;
        else
            parent->left = node;
    }
    else
    {
        tree->root = node;
    }

    itr = node;

    int exit = 0;

    //fix balance
    while(parent && !exit)
    {
        if(itr == parent->left)
        {
           switch (parent->balance)
           {
               case 1:
                   parent->balance = 0;
                   exit = 1;
                   break;

               case 0:
                   parent->balance = -1;
                   break;

               case -1:
                   //fixup left unbalance
                   if(parent->left->balance == -1)
                   {
                        parent = avl_rightRotation(tree, parent);
                        parent->balance = 0;
                        parent->right->balance = 0;
                   }
                   else
                   {
                        int rb = parent->left->right->balance;
                        avl_leftRotation(tree, parent->left);
                        parent = avl_rightRotation(tree, parent);
                        parent->balance = 0;
                        switch (rb)
                        {
                            case -1:
                                parent->left->balance = 0;
                                parent->right->balance = 1;
                                break;

                            case 0:
                                parent->left->balance = 0;
                                parent->right->balance = 0;
                                break;

                            case 1:
                                parent->left->balance = -1;
                                parent->right->balance = 0;
                                break;

                        }
                   }

                   exit = 1;
                   break;
           }
        }
        else
        {
            switch (parent->balance)
            {
                case -1:
                    parent->balance = 0;
                    exit = 1;
                    break;

                case 0:
                    parent->balance = 1;
                    break;

                case 1:
                    //fixup
                    if(parent->right->balance == 1)
                    {
                        parent = avl_leftRotation(tree, parent);
                        parent->balance = 0;
                        parent->left->balance = 0;
                    }
                    else
                    {
                        int rb = parent->right->left->balance;
                        avl_rightRotation(tree, parent->right);
                        parent = avl_leftRotation(tree, parent);
                        parent->balance = 0;
                        switch (rb)
                        {
                            case -1:
                                parent->left->balance = 0;
                                parent->right->balance = 1;
                                break;

                            case 0:
                                parent->left->balance = 0;
                                parent->right->balance = 0;
                                break;

                            case 1:
                                parent->left->balance = -1;
                                parent->right->balance = 0;
                                break;
                        }
                    }

                    exit = 1;
                    break;

            }
        }

        itr = parent;
        parent = itr->parent;
    }

    *inserted = 1;

    return node;
}


AvlNode *avl_search(AvlTree *tree, char *data)
{
    AvlNode *itr = tree->root;

    while (itr && itr != AVL_NIL)
    {
        int cmp = strcmp(data, itr->data);
        if (cmp == 0)
            break;
        else
            itr = (cmp > 0) ? itr->right : itr->left;
    }
    return itr != AVL_NIL ? itr : NULL;
}


AvlNode *alv_treeMin(AvlNode *tree)
{
    while (tree->left != AVL_NIL)
        tree = tree->left;
    return tree;
}


void avl_delete(AvlTree* tree, AvlNode *node)
{
    if(!node)
        return;

    AvlNode *target, *itr, *parent = NULL;

    if(node->left == AVL_NIL || node->right == AVL_NIL)
    {
        target = node;
    }
    else
    {
        //swap data
        target = alv_treeMin(node->right);
        char *dt = node->data;
        node->data = target->data;
        target->data = dt;
        node->relations = target->relations;
    }

    itr = target;
    parent = target->parent;

    int exit = 0, old = 0;
    //recalculate balance
    while(parent && !exit)
    {
        if(itr == parent->left)
        {
            switch (parent->balance)
            {
                case -1:
                    parent->balance = 0;
                    break;

                case 0:
                    parent->balance = 1;
                    exit = 1;
                    break;

                case 1:
                    //fixup

                    switch (parent->right->balance)
                    {
                        case 1:
                            parent = avl_leftRotation(tree, parent);
                            parent->balance = 0;
                            parent->left->balance = 0;
                            break;

                        case 0:
                            parent = avl_leftRotation(tree, parent);
                            parent->balance = -1;
                            parent->left->balance = 1;
                            break;

                        case -1:
                            old = parent->right->left->balance;
                            avl_rightRotation(tree, parent->right);
                            parent = avl_leftRotation(tree, parent);
                            parent->balance = 0;

                            switch(old)
                            {
                                case -1:
                                    parent->left->balance = 0;
                                    parent->right->balance = 1;
                                    break;
                                case 0:
                                    parent->left->balance = 0;
                                    parent->right->balance = 0;
                                    break;
                                case 1:
                                    parent->left->balance = -1;
                                    parent->right->balance = 0;
                                    break;
                            }

                            break;
                    }

                    if (parent->balance == -1)
                        exit = 1;

                    break;
            }
        }
        else
        {
            switch (parent->balance)
            {
                case 1:
                    parent->balance = 0;
                    break;

                case 0:
                    parent->balance = -1;
                    exit = 1;
                    break;

                case -1:
                    //fixup

                    switch (parent->left->balance)
                    {
                        case -1:
                            parent = avl_rightRotation(tree, parent);
                            parent->balance = 0;
                            parent->right->balance = 0;
                            break;

                        case 0:
                            parent = avl_rightRotation(tree, parent);
                            parent->balance = 1;
                            parent->right->balance = -1;
                            break;

                        case 1:
                            old = parent->left->right->balance;
                            avl_leftRotation(tree, parent->left);
                            parent = avl_rightRotation(tree, parent);
                            parent->balance = 0;

                            switch(old)
                            {
                                case -1:
                                    parent->left->balance = 0;
                                    parent->right->balance = 1;
                                    break;
                                case 0:
                                    parent->left->balance = 0;
                                    parent->right->balance = 0;
                                    break;
                                case 1:
                                    parent->left->balance = -1;
                                    parent->right->balance = 0;
                                    break;
                            }

                            break;
                    }

                    if (parent->balance == 1)
                        exit = 1;

                    break;
            }
        }

        itr = parent;
        parent = itr->parent;
    }

    //do actual deletion
    itr = target->left == AVL_NIL ? target->right : target->left;

    if(itr != AVL_NIL)
        itr->parent = target->parent;

    if(target->parent)
    {

        if (target == target->parent->right)
            target->parent->right = itr;
        else
            target->parent->left = itr;
    }
    else
    {
        tree->root = itr;
    }

    free(target->data);
    free(target);

    if(tree->root == AVL_NIL) tree->root = NULL;

}


void avl_clean(AvlTree *tree)
{
    int used = 1;
    static AvlNode *stack[30];

    stack[0] = tree->root;
    if (!stack[0])
        return;
    AvlNode *p;
    while (used > 0)
    {
        p = stack[used - 1];
        used--;
        if (p->right != AVL_NIL)
        {
            stack[used] = p->right;
            used++;
        }
        if (p->left != AVL_NIL)
        {
            stack[used] = p->left;
            used++;
        }
        free(p->data);
        free(p);
    }
}

