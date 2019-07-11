#include <stdlib.h>


typedef struct
{
    char *from;
    char *to;
    char *rel;
    void *tree_node;
} RelationStorageData;

typedef struct s_RelationStorageNode
{
    RelationStorageData *data;
    int color;
    struct s_RelationStorageNode *parent, *right, *left;
} RelationStorageNode;

typedef struct
{
    RelationStorageNode *root;
} RelationStorageTree;



static inline RelationStorageData *rst_allocate(char *from, char *to, char *rel)
{
    RelationStorageData *dt = malloc(sizeof(RelationStorageData));
    dt->from = from;
    dt->to = to;
    dt->rel = rel;

    return dt;
}

static inline int rst_compare(RelationStorageData *x, char *from, char *to, char *rel)
{
    if (from == x->from && to == x->to && rel == x->rel)
        return 0;

    return strcmp(from, x->from) + strcmp(to, x->to) + strcmp(rel, x->rel);
}

static inline void rst_deallocate(RelationStorageData *data)
{

    free(data);
}



static RelationStorageNode *rst_liear_stack[30];
RelationStorageTree *rst_init()
{
    RelationStorageTree *t = malloc(sizeof(RelationStorageTree));
    t->root = NULL;
    return t;
}
static RelationStorageNode rst_sentinel = {{0, 0, 0, 0}, 0, 0, &rst_sentinel, &rst_sentinel};
static inline void rst_leftRotation(RelationStorageTree *tree, RelationStorageNode *x)
{
    RelationStorageNode *y = x->right;
    x->right = y->left;
    if (y->left != &rst_sentinel)
        y->left->parent = x;
    if (y != &rst_sentinel)
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
    if (x != &rst_sentinel)
        x->parent = y;
}
static inline void rst_rightRotation(RelationStorageTree *tree, RelationStorageNode *x)
{
    RelationStorageNode *y = x->left;
    x->left = y->right;
    if (y->right != &rst_sentinel)
        y->right->parent = x;
    if (y != &rst_sentinel)
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
    if (x != &rst_sentinel)
        x->parent = y;
}
static inline void rst_insertFix(RelationStorageTree *tree, RelationStorageNode *x)
{
    RelationStorageNode *y;
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
                    rst_leftRotation(tree, x);
                }
                x->parent->color = 0;
                x->parent->parent->color = 1;
                rst_rightRotation(tree, x->parent->parent);
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
                    rst_rightRotation(tree, x);
                }
                x->parent->color = 0;
                x->parent->parent->color = 1;
                rst_leftRotation(tree, x->parent->parent);
            }
        }
    }
    tree->root->color = 0;
}
RelationStorageNode *rst_insert(RelationStorageTree *tree, char *from, char *to, char *rel, int *inserted)
{
    int cmp = 0;
    RelationStorageNode *parent = NULL, *itr = tree->root;
    while (itr && itr != &rst_sentinel)
    {
        cmp = rst_compare(itr->data, from, to, rel);
        if (cmp == 0)
        {
            *inserted = 0;
            return itr;
        }
        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;
    }
    RelationStorageNode *node = malloc(sizeof(RelationStorageNode));
    node->data = rst_allocate(from, to, rel);
    node->data->tree_node = node;
    node->color = 1;
    node->left = &rst_sentinel;
    node->right = &rst_sentinel;
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
    rst_insertFix(tree, node);
    *inserted = 1;
    return node;
}
RelationStorageNode *rst_search(RelationStorageTree *tree, char *from, char *to, char *rel)
{
    RelationStorageNode *itr = tree->root;
    while (itr && itr != &rst_sentinel)
    {
        int cmp = rst_compare(itr->data, from, to, rel);
        if (cmp == 0)
            break;
        else
            itr = (cmp > 0) ? itr->right : itr->left;
    }
    return itr != &rst_sentinel ? itr : NULL;
}
RelationStorageNode *rst_treeMin(RelationStorageNode *tree)
{
    while (tree->left != &rst_sentinel)
        tree = tree->left;
    return tree;
}
static inline void rst_deleteFix(RelationStorageTree *tree, RelationStorageNode *x)
{
    RelationStorageNode *w;
    while (x != tree->root && x->color == 0)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            if (w->color == 1)
            {
                w->color = 0;
                x->parent->color = 1;
                rst_leftRotation(tree, x->parent);
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
                    rst_rightRotation(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->right->color = 0;
                rst_leftRotation(tree, x->parent);
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
                rst_rightRotation(tree, x->parent);
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
                    rst_leftRotation(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->left->color = 0;
                rst_rightRotation(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = 0;
}
void rst_delete(RelationStorageTree *tree, RelationStorageNode *z)
{
    if (!z)
        return;
    RelationStorageNode *x, *y;
    if (z->left == &rst_sentinel || z->right == &rst_sentinel)
    {
        y = z;
    }
    else
    {
        y = rst_treeMin(z->right);
    }
    if (y->left != &rst_sentinel)
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
        RelationStorageData *temp = z->data;
        z->data = y->data;
        y->data = temp;
        z->data->tree_node = z; // assign new address of node
    }
    if (y->color == 0)
        rst_deleteFix(tree, x);
    rst_deallocate(y->data);
    free(y);

    if(tree->root == &rst_sentinel)
        tree->root = NULL;
}
void rst_clean(RelationStorageTree *tree)
{
    int used = 1;
    rst_liear_stack[0] = tree->root;
    if (!rst_liear_stack[0])
        return;
    RelationStorageNode *p;
    while (used > 0)
    {
        p = rst_liear_stack[used - 1];
        used--;
        if (p->right != &rst_sentinel)
        {
            rst_liear_stack[used] = p->right;
            used++;
        }
        if (p->left != &rst_sentinel)
        {
            rst_liear_stack[used] = p->left;
            used++;
        }
        rst_deallocate(p->data);
        free(p);
    }
}
void rst_count(RelationStorageTree *tree)
{
    int count = 0;
    int used = 1;
    rst_liear_stack[0] = tree->root;
    RelationStorageNode *p;
    while (used > 0)
    {
        p = rst_liear_stack[used - 1];
        used--;
        if (p->right != &rst_sentinel)
        {
            rst_liear_stack[used] = p->right;
            used++;
        }
        if (p->left != &rst_sentinel)
        {
            rst_liear_stack[used] = p->left;
            used++;
        }
        count++;
    }
    printf("Tree elements: %d", count);
}
