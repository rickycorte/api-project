#include <stdlib.h>

/**************************************************************
 *
 * TYPES AND VARS
 *
 ***************************************************************/


typedef void *(*rb_allocator)(void *);
typedef int (*rb_comparator)(void *, void *);
typedef  void (*rb_deallocator)(void *);


typedef struct s_RBTNode
{
    void *data;
    char color;
    struct s_RBTNode *parent, *right, *left;
} RBTNode;


typedef struct
{
    RBTNode *root;
    rb_allocator allocator;
    rb_comparator comparator;
    rb_deallocator deallocator;

} RBTree;


static RBTNode *rb_liear_stack[30];

static RBTNode rb_sentinel = {0, 0, 0, &rb_sentinel, &rb_sentinel};


/**************************************************************
 *
 * INIT
 *
 ***************************************************************/


RBTree *rb_init(rb_allocator alloc, rb_comparator comp, rb_deallocator deall)
{
    RBTree *t = malloc(sizeof(RBTree));

    t->root = NULL;
    t->allocator = alloc;
    t->deallocator = deall;
    t->comparator = comp;

    return t;
}


/**************************************************************
 *
 * ROTATIONS
 *
 ***************************************************************/

static inline void rb_leftRotation(RBTree *tree, RBTNode *x)
{
    RBTNode *y = x->right;
    x->right = y->left;

    if (y->left != &rb_sentinel)
        y->left->parent = x;

    if (y != &rb_sentinel)
        y->parent = x->parent;

    if (x->parent)
    {
        if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    }
    else
    {
        tree->root = y;
    }

    y->left = x;

    if (x != &rb_sentinel)
        x->parent = y;
}


static inline void rb_rightRotation(RBTree *tree, RBTNode *x)
{
    RBTNode *y = x->left;
    x->left = y->right;

    if (y->right != &rb_sentinel)
        y->right->parent = x;

    if (y != &rb_sentinel)
        y->parent = x->parent;

    if (x->parent)
    {
        if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    }
    else
    {
        tree->root = y;
    }

    y->right = x;

    if (x != &rb_sentinel)
        x->parent = y;
}

/**************************************************************
 *
 * INSERT
 *
 ***************************************************************/

static inline void rb_insertFix(RBTree *tree, RBTNode *x)
{
    RBTNode *y;
    while (x != tree->root && x->parent->color == 1)
    {
        if (x->parent == x->parent->parent->left)
        {
            y = x->parent->parent->right;

            if (y->color == 1)
            {
                x->parent->color = 0;
                y->color = 0;
                x->parent->parent->color = 1;
                x = x->parent->parent;
            }
            else
            {
                if (x == x->parent->right)
                {
                    x = x->parent;
                    rb_leftRotation(tree, x);
                }

                x->parent->color = 0;
                x->parent->parent->color = 1;
                rb_rightRotation(tree, x->parent->parent);
            }
        }
        else
        {
            y = x->parent->parent->left;

            if (y->color == 1)
            {
                x->parent->color = 0;
                y->color = 0;
                x->parent->parent->color = 1;
                x = x->parent->parent;
            }
            else
            {
                if (x == x->parent->left)
                {
                    x = x->parent;
                    rb_rightRotation(tree, x);
                }

                x->parent->color = 0;
                x->parent->parent->color = 1;
                rb_leftRotation(tree, x->parent->parent);
            }
        }
    }
    tree->root->color = 0;
}

/**
 * Insert item in tree
 * @param tree tree where insert
 * @param data data to insert (passed to allocator)
 * @param inserted 1 if inseted, 0 on duplicate
 * @return pointer to inserted element or just existing element
 */
RBTNode *rb_insert(RBTree *tree, void *data, int *inserted)
{
    int cmp = 0;
    RBTNode *parent = NULL, *itr = tree->root;

    while (itr && itr != &rb_sentinel)
    {
        cmp = tree->comparator(data, itr->data);

        if (cmp == 0)
        {
            //duplicate
            *inserted = 0;
            return itr;
        }
        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;
    }

    // no duplicate create new

    RBTNode *node = malloc(sizeof(RBTNode));
    node->data = tree->allocator(data);
    node->color = 1;
    node->left = &rb_sentinel;
    node->right = &rb_sentinel;
    node->parent = parent;

    if (parent)
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


    rb_insertFix(tree, node);
    *inserted = 1;

    return node;
}

/**************************************************************
 *
 * SEARCH
 *
 ***************************************************************/

/**
 * Search if element is in the tree
 * @param tree tree where search
 * @param data to search (passed to comparator)
 * @return pointer to node, NULL if not found
 */
RBTNode *rb_search(RBTree *tree, void *data)
{
    RBTNode *itr = tree->root;

    while (itr && itr != &rb_sentinel)
    {
        int cmp = tree->comparator(data, itr->data);
        if (cmp == 0)
            break;
        else
            itr = (cmp > 0) ? itr->right : itr->left;
    }
    return itr != &rb_sentinel ? itr : NULL;
}

/**************************************************************
 *
 * DELETE
 *
 ***************************************************************/

RBTNode *rb_treeMin(RBTNode *tree)
{
    while (tree->left != &rb_sentinel)
        tree = tree->left;
    return tree;
}

static inline void rb_deleteFix(RBTree *tree, RBTNode *x)
{
    RBTNode *w;

    while (x != tree->root && x->color == 0)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;

            if (w->color == 1)
            {
                w->color = 0;
                x->parent->color = 1;
                rb_leftRotation(tree, x->parent);
                w = x->parent->right;
            }

            if (w->left->color == 0 && w->right->color == 0)
            {
                w->color = 1;
                x = x->parent;
            }
            else
            {
                if (w->right->color == 0)
                {
                    w->left->color = 0;
                    w->color = 1;
                    rb_rightRotation(tree, w);
                    w = x->parent->right;
                }

                w->color = x->parent->color;
                x->parent->color = 0;
                w->right->color = 0;
                rb_leftRotation(tree, x->parent);
                x = tree->root;
            }
        }
        else
        {
            w = x->parent->left;

            if (w->color == 1)
            {
                w->color = 0;
                x->parent->color = 1;
                rb_rightRotation(tree, x->parent);
                w = x->parent->left;
            }

            if (w->right->color == 0 && w->left->color == 0)
            {
                w->color = 1;
                x = x->parent;
            }
            else
            {
                if (w->left->color == 0)
                {
                    w->right->color = 0;
                    w->color = 1;
                    rb_leftRotation(tree, w);
                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = 0;
                w->left->color = 0;
                rb_rightRotation(tree, x->parent);
                x = tree->root;
            }
        }
    }

    x->color = 0;
}

/**
 * Delete an element from the tree
 *
 * @param tree tree where delete
 * @param z pointer to the element of the tree to delete
 *
 */
void rb_delete(RBTree *tree, RBTNode *z)
{
    if (!z)
        return;

    RBTNode *x, *y;

    if (z->left == &rb_sentinel || z->right == &rb_sentinel)
    {
        y = z;
    }
    else
    {
        y = rb_treeMin(z->right);
    }

    if (y->left != &rb_sentinel)
        x = y->left;
    else
        x = y->right;

    x->parent = y->parent;

    if (y->parent)
    {
        if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
    }
    else
    {
        tree->root = x;
    }

    if (y != z)
    {
        char *temp = z->data;
        z->data = y->data;
        y->data = temp;
    }

    if (y->color == 0)
        rb_deleteFix(tree, x);


    tree->deallocator(y->data);
    free(y);

    //fix root to be NULL
    if (tree->root == &rb_sentinel)
        tree->root = NULL;
}

/**************************************************************
 *
 * CLEAN UP
 *
 ***************************************************************/

/**
 * Delete all elements of the tree and the tree
 * @param tree
 */
void rb_clean(RBTree *tree)
{
    int used = 1;

    rb_liear_stack[0] = tree->root;

    if (!rb_liear_stack[0])
        return;

    RBTNode *p;

    while (used > 0)
    {
        p = rb_liear_stack[used - 1];
        used--;

        if (p->right != &rb_sentinel)
        {
            rb_liear_stack[used] = p->right;
            used++;
        }

        if (p->left != &rb_sentinel)
        {
            rb_liear_stack[used] = p->left;
            used++;
        }

        tree->deallocator(p->data);
        free(p);
    }

    free(tree);
}

/**************************************************************
 *
 * MORE
 *
 ***************************************************************/

/**
 * Count element of tree
 * debug function :3
 *
 * @param tree
 */
void rb_count(RBTree *tree)
{
    int count = 0;
    int used = 1;
    rb_liear_stack[0] = tree->root;
    RBTNode *p;
    while (used > 0)
    {
        p = rb_liear_stack[used - 1];
        used--;
        if (p->right != &rb_sentinel)
        {
            rb_liear_stack[used] = p->right;
            used++;
        }
        if (p->left != &rb_sentinel)
        {
            rb_liear_stack[used] = p->left;
            used++;
        }
        count++;
    }
    printf("Tree elements: %d\n", count);
}