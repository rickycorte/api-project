#include "relation_storage_tree.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define RSTNIL &sentinel

static RelationNameNode sentinel = { 0, 0, ET_BLACK, 0, RSTNIL, RSTNIL };


static RelationNameNode* rht_liear_stack[30]; // can iterate tree with 2^30 elements

RelationNameTree *rht_init()
{
    RelationNameTree *t = malloc(sizeof(RelationNameTree));
    t->root = NULL;
    t->insert_count = 0;

    return t;
}

/**************************************************************
 *
 * ROTATIONS
 *
 ***************************************************************/

static inline void rht_leftRotation(RelationNameTree *tree, RelationNameNode *x)
{
    RelationNameNode *y = x->right;

    x->right = y->left;
    if(y->left != RSTNIL)
        y->left->parent = x;

    if(y != RSTNIL) y->parent = x->parent;
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
    if(x != RSTNIL)
        x->parent = y;

}


static inline void rht_rightRotation(RelationNameTree *tree, RelationNameNode *x)
{
    RelationNameNode *y = x->left;

    x->left = y->right;
    if(y->right != RSTNIL)
        y->right->parent = x;

    if(y != RSTNIL) y->parent = x->parent;
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
    if(x != RSTNIL)
        x->parent = y;
}


/**************************************************************
 *
 * INSERT
 *
 ***************************************************************/

static inline void rht_insertFix(RelationNameTree *tree, RelationNameNode *x)
{
    RelationNameNode *y;

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
                    rht_leftRotation(tree, x);
                }

                x->parent->color = ET_BLACK;
                x->parent->parent->color = ET_RED;

                rht_rightRotation(tree, x->parent->parent);
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
                    rht_rightRotation(tree, x);
                }

                x->parent->color = ET_BLACK;
                x->parent->parent->color = ET_RED;

                rht_leftRotation(tree, x->parent->parent);
            }
        }
    }

    tree->root->color = ET_BLACK;
}



RelationNameNode *rht_insert(RelationNameTree *tree, char *entity_name, int *inserted)
{
    // not null root here
    int cmp = 0;
    RelationNameNode *parent = NULL, *itr = tree->root;

    while(itr && itr != RSTNIL)
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

    RelationNameNode *node = malloc(sizeof(RelationNameNode));
    node->name = entity_name;
    node->color = ET_RED; //base colo res
    node->left = RSTNIL;
    node->right = RSTNIL;
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
    rht_insertFix(tree, node);

    *inserted = 1;
    return node;
}


/**************************************************************
 *
 * SEARCH
 *
 ***************************************************************/


RelationNameNode *rht_search(RelationNameTree *tree, char *entity_name)
{
    RelationNameNode *itr = tree->root;
    while(itr && itr != RSTNIL)
    {
        int cmp = strcmp(entity_name, itr->name);
        if(cmp == 0)
            break;
        else
            itr = (cmp > 0)? itr->right : itr->left;
    }

    return itr != RSTNIL ? itr : NULL;
}


/**************************************************************
 *
 * DELETE
 *
 ***************************************************************/

RelationNameNode *rht_treeMin(RelationNameNode *tree)
{
    while(tree->left != RSTNIL)
        tree = tree->left;

    return tree;
}




static inline void rht_deleteFix(RelationNameTree *tree, RelationNameNode *x)
{
    RelationNameNode *w;

    while(x != tree->root && x->color == ET_BLACK)
    {

        if(x == x->parent->left)
        {
            w = x->parent->right;

            if (w->color == ET_RED)
            {
                w->color = ET_BLACK;
                x->parent->color = ET_RED;

                rht_leftRotation(tree, x->parent);
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

                    rht_rightRotation(tree, w);
                    w = x->parent->right;
                }

                w->color = x->parent->color;
                x->parent->color = ET_BLACK;
                w->right->color = ET_BLACK;

                rht_leftRotation(tree, x->parent);

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

                rht_rightRotation(tree, x->parent);
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

                    rht_leftRotation(tree, w);
                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = ET_BLACK;
                w->left->color = ET_BLACK;

                rht_rightRotation(tree, x->parent);

                x = tree->root;
            }
        }
    }

    x->color = ET_BLACK;
}


void rht_delete(RelationNameTree *tree, char *entity_name)
{
    RelationNameNode *z = rht_search(tree, entity_name);
    if(!z)
        return;

    RelationNameNode *x, *y;

    if(z->left == RSTNIL || z->right == RSTNIL)
    {
        y = z;
    }
    else
    {
        y = et_treeMin(z->right);
    }

    if(y->left != RSTNIL)
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
        rht_deleteFix(tree, x);

    free(y->name);
    free(y);
}


/**************************************************************
 *
 * CLEAN UP
 *
 ***************************************************************/


void rht_clean(RelationNameTree *tree)
{

    int used = 1;
    rht_liear_stack[0] = tree->root;
    RelationNameNode *p;

    while(used > 0) // stack not empty
    {
        p = rht_liear_stack[used-1];
        used--;

        if(p->right != RSTNIL)
        {
            rht_liear_stack[used] = p->right;
            used++;
        }
        if(p->left != RSTNIL)
        {
            rht_liear_stack[used] = p->left;
            used++;
        }

        free(p->name);
        free(p);

    }

}


void rht_count(RelationNameTree *tree)
{

    int count = 0;
    int used = 1;
    rht_liear_stack[0] = tree->root;
    RelationNameNode *p;

    while(used > 0) // stack not empty
    {
        p = rht_liear_stack[used-1];
        used--;

        if(p->right != RSTNIL)
        {
            rht_liear_stack[used] = p->right;
            used++;
        }
        if(p->left != RSTNIL)
        {
            rht_liear_stack[used] = p->left;
            used++;
        }

        count++;
    }

    printf("Tree elements: %d", count);

}
