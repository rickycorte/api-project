#include "relation_holder_tree.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define RHTNIL &sentinel

static RelationStorageNode sentinel = { 0, 0, ET_BLACK, 0, RHTNIL, RHTNIL };


static RelationStorageNode* rst_liear_stack[30]; // can iterate tree with 2^30 elements

RelationStorageTree *rst_init()
{
    RelationStorageTree *t = malloc(sizeof(RelationStorageTree));
    t->root = NULL;
    t->insert_count = 0;

    return t;
}

/**************************************************************
 *
 * ROTATIONS
 *
 ***************************************************************/

static inline void rst_leftRotation(RelationStorageTree *tree, RelationStorageNode *x)
{
    RelationStorageNode *y = x->right;

    x->right = y->left;
    if(y->left != RHTNIL)
        y->left->parent = x;

    if(y != RHTNIL) y->parent = x->parent;
    if(x->parent)
    {
        if(x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    }
    else
    {
        tree->root = y;
    }

    y->left = x;
    if(x != RHTNIL)
        x->parent = y;

}


static inline void rst_rightRotation(RelationStorageTree *tree, RelationStorageNode *x)
{
    RelationStorageNode *y = x->left;

    x->left = y->right;
    if(y->right != RHTNIL)
        y->right->parent = x;

    if(y != RHTNIL) y->parent = x->parent;
    if(x->parent)
    {
        if(x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    }
    else
    {
        tree->root = y;
    }

    y->right = x;
    if(x != RHTNIL)
        x->parent = y;
}


/**************************************************************
 *
 * INSERT
 *
 ***************************************************************/

static inline void rst_insertFix(RelationStorageTree *tree, RelationStorageNode *x)
{
    RelationStorageNode *y;

    while(x != tree->root && x->parent->color == ET_RED)
    {

        if(x->parent == x->parent->parent->left)
        {
            y = x->parent->parent->right;

            if(y->color == ET_RED)
            {
                //colorflip
                x->parent->color = ET_BLACK;
                y->color = ET_BLACK;
                x->parent->parent->color = ET_RED;

                x = x->parent->parent;
            }
            else
            {
                if(x == x->parent->right)
                {
                    x = x->parent;
                    rst_leftRotation(tree, x);
                }

                x->parent->color = ET_BLACK;
                x->parent->parent->color = ET_RED;

                rst_rightRotation(tree, x->parent->parent);
            }
        }
        else
        {
            y = x->parent->parent->left;

            if(y->color == ET_RED)
            {
                //colorflip
                x->parent->color = ET_BLACK;
                y->color = ET_BLACK;
                x->parent->parent->color = ET_RED;

                x = x->parent->parent;
            }
            else
            {
                if(x == x->parent->left)
                {
                    x = x->parent;
                    rst_rightRotation(tree, x);
                }

                x->parent->color = ET_BLACK;
                x->parent->parent->color = ET_RED;

                rst_leftRotation(tree, x->parent->parent);
            }
        }
    }

    tree->root->color = ET_BLACK;
}



RelationStorageNode *rst_insert(RelationStorageTree *tree, char *entity_name, int *inserted)
{
    // not null root here
    int cmp = 0;
    RelationStorageNode *parent = NULL, *itr = tree->root;

    while(itr && itr != RHTNIL)
    {
        cmp = strcmp(entity_name, itr->name);

        if(cmp == 0) // found duplicate
        {
            *inserted = 0;
            return itr;
        }

        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;
    }

    RelationStorageNode *node = malloc(sizeof(RelationStorageNode));
    node->name = entity_name;
    node->color = ET_RED; //base colo res
    node->left = RHTNIL;
    node->right = RHTNIL;
    node->parent = parent;

    node->insert_order = tree->insert_count;
    tree->insert_count++;

    if(parent)
    {
        if (cmp > 0)
            parent->right = node;
        else
            parent->left = node;
    }
    else
    {
        tree->root = node;
    }

    //fixup
    rst_insertFix(tree, node);

    *inserted = 1;
    return node;
}


/**************************************************************
 *
 * SEARCH
 *
 ***************************************************************/


RelationStorageNode *rst_search(RelationStorageTree *tree, char *entity_name)
{
    RelationStorageNode *itr = tree->root;
    while(itr && itr != RHTNIL)
    {
        int cmp = strcmp(entity_name, itr->name);
        if(cmp == 0)
            break;
        else
        itr = (cmp > 0)? itr->right : itr->left;
    }

    return itr != RHTNIL ? itr : NULL;
}


/**************************************************************
 *
 * DELETE
 *
 ***************************************************************/

RelationStorageNode *rst_treeMin(RelationStorageNode *tree)
{
    while(tree->left != RHTNIL)
        tree = tree->left;

    return tree;
}




static inline void rst_deleteFix(RelationStorageTree *tree, RelationStorageNode *x)
{
    RelationStorageNode *w;

    while(x != tree->root && x->color == ET_BLACK)
    {

        if(x == x->parent->left)
        {
            w = x->parent->right;

            if (w->color == ET_RED)
            {
                w->color = ET_BLACK;
                x->parent->color = ET_RED;

                rst_leftRotation(tree, x->parent);
                w = x->parent->right;
            }

            if (w->left->color == ET_BLACK && w->right->color == ET_BLACK)
            {
                w->color = ET_RED;
                x = x->parent;
            }
            else
            {
                if(w->right->color == ET_BLACK)
                {
                    w->left->color = ET_BLACK;
                    w->color = ET_RED;

                    rst_rightRotation(tree, w);
                    w = x->parent->right;
                }

                w->color = x->parent->color;
                x->parent->color = ET_BLACK;
                w->right->color = ET_BLACK;

                rst_leftRotation(tree, x->parent);

                x = tree->root;
            }

        }
        else
        {
            w = x->parent->left;

            if (w->color == ET_RED)
            {
                w->color = ET_BLACK;
                x->parent->color = ET_RED;

                rst_rightRotation(tree, x->parent);
                w = x->parent->left;
            }

            if (w->right->color == ET_BLACK && w->left->color == ET_BLACK)
            {
                w->color = ET_RED;
                x = x->parent;
            }
            else
            {
                if(w->left->color == ET_BLACK)
                {
                    w->right->color = ET_BLACK;
                    w->color = ET_RED;

                    rst_leftRotation(tree, w);
                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = ET_BLACK;
                w->left->color = ET_BLACK;

                rst_rightRotation(tree, x->parent);

                x = tree->root;
            }
        }
    }

    x->color = ET_BLACK;
}


void rst_delete(RelationStorageTree *tree, char *entity_name)
{
    RelationStorageNode *z = rst_search(tree, entity_name);
    if(!z)
        return;

    RelationStorageNode *x, *y;

    if(z->left == RHTNIL || z->right == RHTNIL)
    {
        y = z;
    }
    else
    {
        y = et_treeMin(z->right);
    }

    if(y->left != RHTNIL)
        x = y->left;
    else
        x = y->right;

    x->parent = y->parent;
    if(y->parent)
    {
        if(y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
    }
    else
    {
        tree->root = x;
    }

    if(y != z)
    {
        char *temp = z->name;
        z->name = y->name;
        y->name = temp;
    }

    if(y->color == ET_BLACK)
        rst_deleteFix(tree, x);

    free(y->name);
    free(y);
}


/**************************************************************
 *
 * CLEAN UP
 *
 ***************************************************************/


void rst_clean(RelationStorageTree *tree)
{

    int used = 1;
    rst_liear_stack[0] = tree->root;
    RelationStorageNode *p;

    while(used > 0) // stack not empty
    {
        p = rst_liear_stack[used-1];
        used--;

        if(p->right != RHTNIL)
        {
            rst_liear_stack[used] = p->right;
            used++;
        }
        if(p->left != RHTNIL)
        {
            rst_liear_stack[used] = p->left;
            used++;
        }

        free(p->name);
        free(p);

    }

}


void rst_count(RelationStorageTree *tree)
{

    int count = 0;
    int used = 1;
    rst_liear_stack[0] = tree->root;
    RelationStorageNode *p;

    while(used > 0) // stack not empty
    {
        p = rst_liear_stack[used-1];
        used--;

        if(p->right != RHTNIL)
        {
            rst_liear_stack[used] = p->right;
            used++;
        }
        if(p->left != RHTNIL)
        {
            rst_liear_stack[used] = p->left;
            used++;
        }

        count++;
    }

    printf("Tree elements: %d", count);

}
