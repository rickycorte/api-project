#include "entity_tree.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ETNIL &sentinel

static EntityNode sentinel = { 0, ET_BLACK, 0, ETNIL, ETNIL };


static EntityNode* et_liear_stack[30]; // can iterate tree with 2^30 elements

EntityTree *et_init()
{
    EntityTree *t = malloc(sizeof(EntityTree));
    t->root = NULL;

    return t;
}

/**************************************************************
 *
 * ROTATIONS
 *
 ***************************************************************/

static inline void et_leftRotation(EntityTree *tree, EntityNode *x)
{
    EntityNode *y = x->right;

    x->right = y->left;
    if(y->left != ETNIL)
        y->left->parent = x;

    if(y != ETNIL) y->parent = x->parent;
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
    if(x != ETNIL)
        x->parent = y;

}


static inline void et_rightRotation(EntityTree *tree, EntityNode *x)
{
    EntityNode *y = x->left;

    x->left = y->right;
    if(y->right != ETNIL)
        y->right->parent = x;

    if(y != ETNIL) y->parent = x->parent;
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
    if(x != ETNIL)
        x->parent = y;
}


/**************************************************************
 *
 * INSERT
 *
 ***************************************************************/

static inline void et_insertFix(EntityTree *tree, EntityNode *x)
{
    EntityNode *y;

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
                    et_leftRotation(tree, x);
                }

                x->parent->color = ET_BLACK;
                x->parent->parent->color = ET_RED;

                et_rightRotation(tree, x->parent->parent);
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
                    et_rightRotation(tree, x);
                }

                x->parent->color = ET_BLACK;
                x->parent->parent->color = ET_RED;

                et_leftRotation(tree, x->parent->parent);
            }
        }
    }

    tree->root->color = ET_BLACK;
}



int et_insert(EntityTree *tree, char *entity_name)
{
    // not null root here
    int cmp = 0;
    EntityNode *parent = NULL, *itr = tree->root;

    while(itr && itr != ETNIL)
    {
        cmp = strcmp(entity_name, itr->name);

        if(cmp == 0) // found duplicate
        {
            return 0;
        }

        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;
    }

    EntityNode *node = malloc(sizeof(EntityNode));
    node->name = entity_name;
    node->color = ET_RED; //base colo res
    node->left = ETNIL;
    node->right = ETNIL;
    node->parent = parent;

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
    et_insertFix(tree, node);

    return 1;
}


/**************************************************************
 *
 * SEARCH
 *
 ***************************************************************/


EntityNode *et_search(EntityTree *tree, char *entity_name)
{
    EntityNode *itr = tree->root;
    while(itr && itr != ETNIL)
    {
        int cmp = strcmp(entity_name, itr->name);
        if(cmp == 0)
            break;
        else
        itr = (cmp > 0)? itr->right : itr->left;
    }

    return itr != ETNIL ? itr : NULL;
}


/**************************************************************
 *
 * DELETE
 *
 ***************************************************************/

EntityNode *et_treeMin(EntityNode *tree)
{
    while(tree->left != ETNIL)
        tree = tree->left;

    return tree;
}




static inline void et_deleteFix(EntityTree *tree, EntityNode *x)
{
    EntityNode *w;

    while(x != tree->root && x->color == ET_BLACK)
    {

        if(x == x->parent->left)
        {
            w = x->parent->right;

            if (w->color == ET_RED)
            {
                w->color = ET_BLACK;
                x->parent->color = ET_RED;

                et_leftRotation(tree, x->parent);
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

                    et_rightRotation(tree, w);
                    w = x->parent->right;
                }

                w->color = x->parent->color;
                x->parent->color = ET_BLACK;
                w->right->color = ET_BLACK;

                et_leftRotation(tree, x->parent);

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

                et_rightRotation(tree, x->parent);
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

                    et_leftRotation(tree, w);
                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = ET_BLACK;
                w->left->color = ET_BLACK;

                et_rightRotation(tree, x->parent);

                x = tree->root;
            }
        }
    }

    x->color = ET_BLACK;
}


void et_delete(EntityTree *tree, char *entity_name)
{
    EntityNode *z = et_search(tree, entity_name);
    if(!z)
        return;

    EntityNode *x, *y;

    if(z->left == ETNIL || z->right == ETNIL)
    {
        y = z;
    }
    else
    {
        y = et_treeMin(z->right);
    }

    if(y->left != ETNIL)
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
        et_deleteFix(tree, x);

    free(y->name);
    free(y);
}


/**************************************************************
 *
 * CLEAN UP
 *
 ***************************************************************/


void et_clean(EntityTree *tree)
{

    int used = 1;
    et_liear_stack[0] = tree->root;
    EntityNode *p;

    while(used > 0) // stack not empty
    {
        p = et_liear_stack[used-1];
        used--;

        if(p->right != ETNIL)
        {
            et_liear_stack[used] = p->right;
            used++;
        }
        if(p->left != ETNIL)
        {
            et_liear_stack[used] = p->left;
            used++;
        }

        free(p->name);
        free(p);

    }

}



/**************************************************************
 *
 * DEBUG
 *
 ***************************************************************/

#define KRED  "\x1B[31m"
#define KBLK  "\x1B[37m"
#define KRESET "\033[0m"

// compute height of the tree
int et_getHeight(EntityNode *root) {
    if (root == NULL || root == ETNIL)
        return 0;

    // find the height of each subtree
    int lh = et_getHeight(root->left);
    int rh = et_getHeight(root->right);

    return 1 + (rh > lh ? rh : lh);
}


void et_print_sub_tree(EntityNode *tree, int rb_height)
{
    if(!tree || tree == ETNIL)
    {
        return;
    }

    et_print_sub_tree(tree->left, rb_height-1);

    for(int i = 0; i < rb_height; i++)
    {
        printf("          ");
    }
    printf("%s%03s ----+%s\n", tree->color? KRED : KBLK, tree->name, KRESET);
    et_print_sub_tree(tree->right, rb_height-1);
}

void et_print(EntityNode *root)
{
    if(!root)
    {
        printf("Empty\n");
        return;
    }

    int rb_height = et_getHeight(root);
    printf("rb height: %d\n", rb_height);
    et_print_sub_tree(root, rb_height);

}


void et_count(EntityTree *tree)
{

    int count = 0;
    int used = 1;
    et_liear_stack[0] = tree->root;
    EntityNode *p;

    while(used > 0) // stack not empty
    {
        p = et_liear_stack[used-1];
        used--;

        if(p->right != ETNIL)
        {
            et_liear_stack[used] = p->right;
            used++;
        }
        if(p->left != ETNIL)
        {
            et_liear_stack[used] = p->left;
            used++;
        }

        count++;
    }

    printf("Tree elements: %d", count);

}
