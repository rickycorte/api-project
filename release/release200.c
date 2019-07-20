#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

#define INPUT_BUFFER_SIZE 1024
#define REPORT_OUT_QUEUE_SIZE 512
#define REPORT_TREES 50
#define REPORT_BUFFER_SIZE 1024

#ifdef DEBUG
    #define DEBUG_PRINT printf
#else
    #define DEBUG_PRINT(...)
#endif

#ifdef DEBUG
    #define FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define FORCE_INLINE inline
#endif



/**********************************************************
 * 
 * ENTITY TREEE
 *  
 *********************************************************/


typedef struct s_EntityNode
{
    char *data;
    int relations;
    int color;
    struct s_EntityNode *parent, *right, *left;
} EntityNode;
typedef struct
{
    EntityNode *root;
} EntityTree;
static EntityNode *et_liear_stack[30];
EntityTree *et_init()
{
    EntityTree *t = malloc(sizeof(EntityTree));
    t->root = NULL;
    return t;
}
static EntityNode et_sentinel = {0, 0, 0, &et_sentinel, &et_sentinel};
static inline void et_leftRotation(EntityTree *tree, EntityNode *x)
{
    EntityNode *y = x->right;
    x->right = y->left;
    if (y->left != &et_sentinel)
        y->left->parent = x;
    if (y != &et_sentinel)
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
    if (x != &et_sentinel)
        x->parent = y;
}
static inline void et_rightRotation(EntityTree *tree, EntityNode *x)
{
    EntityNode *y = x->left;
    x->left = y->right;
    if (y->right != &et_sentinel)
        y->right->parent = x;
    if (y != &et_sentinel)
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
    if (x != &et_sentinel)
        x->parent = y;
}
static inline void et_insertFix(EntityTree *tree, EntityNode *x)
{
    EntityNode *y;
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
                    et_leftRotation(tree, x);
                }
                x->parent->color = 0;
                x->parent->parent->color = 1;
                et_rightRotation(tree, x->parent->parent);
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
                    et_rightRotation(tree, x);
                }
                x->parent->color = 0;
                x->parent->parent->color = 1;
                et_leftRotation(tree, x->parent->parent);
            }
        }
    }
    tree->root->color = 0;
}
EntityNode *et_insert(EntityTree *tree, char *entity, int *inserted)
{
    int cmp = 0;
    EntityNode *parent = NULL, *itr = tree->root;
    while (itr && itr != &et_sentinel)
    {
        cmp = strcmp(entity, itr->data);
        if (cmp == 0)
        {
            *inserted = 0;
            return itr;
        }
        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;
    }
    EntityNode *node = malloc(sizeof(EntityNode));
    node->data = entity;
    node->color = 1;
    node->left = &et_sentinel;
    node->right = &et_sentinel;
    node->parent = parent;
    node->relations = 0;
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
    et_insertFix(tree, node);
    *inserted = 1;
    return node;
}
EntityNode *et_search(EntityTree *tree, char *entity)
{
    EntityNode *itr = tree->root;

    while (itr && itr != &et_sentinel)
    {
        int cmp = strcmp(entity, itr->data);
        if (cmp == 0)
            break;
        else
            itr = (cmp > 0) ? itr->right : itr->left;
    }
    return itr != &et_sentinel ? itr : NULL;
}
EntityNode *et_treeMin(EntityNode *tree)
{
    while (tree->left != &et_sentinel)
        tree = tree->left;
    return tree;
}
static inline void et_deleteFix(EntityTree *tree, EntityNode *x)
{
    EntityNode *w;
    while (x != tree->root && x->color == 0)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            if (w->color == 1)
            {
                w->color = 0;
                x->parent->color = 1;
                et_leftRotation(tree, x->parent);
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
                    et_rightRotation(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->right->color = 0;
                et_leftRotation(tree, x->parent);
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
                et_rightRotation(tree, x->parent);
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
                    et_leftRotation(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->left->color = 0;
                et_rightRotation(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = 0;
}
void et_delete(EntityTree *tree, EntityNode *z)
{
    if (!z)
        return;
    EntityNode *x, *y;
    if (z->left == &et_sentinel || z->right == &et_sentinel)
    {
        y = z;
    }
    else
    {
        y = et_treeMin(z->right);
    }
    if (y->left != &et_sentinel)
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
        z->relations = y->relations;
    }
    if (y->color == 0)
        et_deleteFix(tree, x);
    free(y->data);
    free(y);
    if (tree->root == &et_sentinel)
        tree->root = NULL;
}
void et_clean(EntityTree *tree)
{
    int used = 1;
    et_liear_stack[0] = tree->root;
    if (!et_liear_stack[0])
        return;
    EntityNode *p;
    while (used > 0)
    {
        p = et_liear_stack[used - 1];
        used--;
        if (p->right != &et_sentinel)
        {
            et_liear_stack[used] = p->right;
            used++;
        }
        if (p->left != &et_sentinel)
        {
            et_liear_stack[used] = p->left;
            used++;
        }
        free(p->data);
        free(p);
    }
}
void et_count(EntityTree *tree)
{
    int count = 0;
    int used = 1;
    et_liear_stack[0] = tree->root;
    EntityNode *p;
    while (used > 0)
    {
        p = et_liear_stack[used - 1];
        used--;
        if (p->right != &et_sentinel)
        {
            et_liear_stack[used] = p->right;
            used++;
        }
        if (p->left != &et_sentinel)
        {
            et_liear_stack[used] = p->left;
            used++;
        }
        count++;
    }
    printf("Tree elements: %d\n", count);
}


/**********************************************************
 * 
 * RELATION NAME TREEE
 *  
 *********************************************************/


static FORCE_INLINE char *rel_allocate(char *data) { return data; }
static FORCE_INLINE int rel_compare(char *x, char *y) { return strcmp(y, x); }
static FORCE_INLINE void rel_deallocate(char *data) { free(data); }

typedef struct s_RelationNameNode
{
    char *data;
    int color;
    int id;
    struct s_RelationNameNode *parent, *right, *left;
} RelationNameNode;

typedef struct
{
    int count;
    RelationNameNode *root;
} RelationNameTree;

static RelationNameNode *rel_liear_stack[30];

RelationNameTree *rel_init()
{
    RelationNameTree *t = malloc(sizeof(RelationNameTree));
    t->root = NULL;
    t->count = 0;
    return t;
}
static RelationNameNode rel_sentinel = {0, 0, 0, 0, &rel_sentinel, &rel_sentinel};
static inline void rel_leftRotation(RelationNameTree *tree, RelationNameNode *x)
{
    RelationNameNode *y = x->right;
    x->right = y->left;
    if (y->left != &rel_sentinel)
        y->left->parent = x;
    if (y != &rel_sentinel)
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
    if (x != &rel_sentinel)
        x->parent = y;
}
static inline void rel_rightRotation(RelationNameTree *tree, RelationNameNode *x)
{
    RelationNameNode *y = x->left;
    x->left = y->right;
    if (y->right != &rel_sentinel)
        y->right->parent = x;
    if (y != &rel_sentinel)
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
    if (x != &rel_sentinel)
        x->parent = y;
}
static inline void rel_insertFix(RelationNameTree *tree, RelationNameNode *x)
{
    RelationNameNode *y;
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
                    rel_leftRotation(tree, x);
                }
                x->parent->color = 0;
                x->parent->parent->color = 1;
                rel_rightRotation(tree, x->parent->parent);
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
                    rel_rightRotation(tree, x);
                }
                x->parent->color = 0;
                x->parent->parent->color = 1;
                rel_leftRotation(tree, x->parent->parent);
            }
        }
    }
    tree->root->color = 0;
}
RelationNameNode *rel_insert(RelationNameTree *tree, char *relation, int *inserted)
{
    int cmp = 0;
    RelationNameNode *parent = NULL, *itr = tree->root;
    while (itr && itr != &rel_sentinel)
    {
        cmp = strcmp(relation, itr->data);
        if (cmp == 0)
        {
            *inserted = 0;
            return itr;
        }
        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;
    }
    RelationNameNode *node = malloc(sizeof(RelationNameNode));
    node->data = relation;
    node->color = 1;
    node->left = &rel_sentinel;
    node->right = &rel_sentinel;
    node->parent = parent;
    node->id = tree->count;

    tree->count += 1;

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
    rel_insertFix(tree, node);
    *inserted = 1;
    return node;
}
RelationNameNode *rel_search(RelationNameTree *tree, char *relation)
{
    RelationNameNode *itr = tree->root;
    while (itr && itr != &rel_sentinel)
    {
        int cmp = rel_compare(itr->data, relation);
        if (cmp == 0)
            break;
        else
            itr = (cmp > 0) ? itr->right : itr->left;
    }
    return itr != &rel_sentinel ? itr : NULL;
}
RelationNameNode *rel_treeMin(RelationNameNode *tree)
{
    while (tree->left != &rel_sentinel)
        tree = tree->left;
    return tree;
}
static inline void rel_deleteFix(RelationNameTree *tree, RelationNameNode *x)
{
    RelationNameNode *w;
    while (x != tree->root && x->color == 0)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            if (w->color == 1)
            {
                w->color = 0;
                x->parent->color = 1;
                rel_leftRotation(tree, x->parent);
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
                    rel_rightRotation(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->right->color = 0;
                rel_leftRotation(tree, x->parent);
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
                rel_rightRotation(tree, x->parent);
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
                    rel_leftRotation(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->left->color = 0;
                rel_rightRotation(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = 0;
}
void rel_delete(RelationNameTree *tree, RelationNameNode *z)
{
    if (!z)
        return;
    RelationNameNode *x, *y;
    if (z->left == &rel_sentinel || z->right == &rel_sentinel)
    {
        y = z;
    }
    else
    {
        y = rel_treeMin(z->right);
    }
    if (y->left != &rel_sentinel)
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
        z->id = y->id;
    }
    if (y->color == 0)
        rel_deleteFix(tree, x);
    free(y->data);
    free(y);
    if (tree->root == &rel_sentinel)
        tree->root = NULL;
}
void rel_clean(RelationNameTree *tree)
{
    int used = 1;
    rel_liear_stack[0] = tree->root;
    if (!rel_liear_stack[0])
        return;
    RelationNameNode *p;
    while (used > 0)
    {
        p = rel_liear_stack[used - 1];
        used--;
        if (p->right != &rel_sentinel)
        {
            rel_liear_stack[used] = p->right;
            used++;
        }
        if (p->left != &rel_sentinel)
        {
            rel_liear_stack[used] = p->left;
            used++;
        }
        free(p->data);
        free(p);
    }
}
void rel_count(RelationNameTree *tree)
{
    int count = 0;
    int used = 1;
    rel_liear_stack[0] = tree->root;
    RelationNameNode *p;
    while (used > 0)
    {
        p = rel_liear_stack[used - 1];
        used--;
        if (p->right != &rel_sentinel)
        {
            rel_liear_stack[used] = p->right;
            used++;
        }
        if (p->left != &rel_sentinel)
        {
            rel_liear_stack[used] = p->left;
            used++;
        }
        count++;
    }
    printf("Tree elements: %d\n", count);
}


/**********************************************************
 * 
 * RELATION STORAGE
 *  
 *********************************************************/


#include <stdlib.h>


typedef struct
{
    char *from;
    char *to;
    char *rel;
    void *tree_node;
    int rel_id;
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



static inline RelationStorageData *rst_allocate(char *from, char *to, char *rel, int rel_id)
{
    RelationStorageData *dt = malloc(sizeof(RelationStorageData));
    dt->from = from;
    dt->to = to;
    dt->rel = rel;
    dt->rel_id = rel_id;
    return dt;
}

static inline int rst_compare(RelationStorageData *x, char *from, char *to, char *rel)
{
    int res = strcmp(from, x->from);
    if(res) return res;
    res = strcmp(to, x->to);
    if(res) return res;
    return strcmp(rel, x->rel);
    //TODO: provare a comparare gli id di relazione per risparmiare una strcmp
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
static RelationStorageNode rst_sentinel = {0, 0, 0, &rst_sentinel, &rst_sentinel};
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
RelationStorageNode *rst_insert(RelationStorageTree *tree, char *from, char *to, char *rel, int rel_id, int *inserted)
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
    node->data = rst_allocate(from, to, rel, rel_id);
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
/**
 * Delete and return relation of deleted node
 * @param tree
 * @param z
 * @return
 */
int rst_delete(RelationStorageTree *tree, RelationStorageNode *z)
{
    if (!z)
        return -1;
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

    int res = y->data->rel_id;

    rst_deallocate(y->data);
    free(y);

    if(tree->root == &rst_sentinel)
        tree->root = NULL;

    return res;
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
    printf("Tree elements: %d\n", count);
}



/**********************************************************
 * 
 * REPORT TREE
 *  
 *********************************************************/

typedef struct s_ReportNode
{
    char *data;
    int count;
    int color;
    struct s_ReportNode *parent, *right, *left;
} ReportNode;
typedef struct
{
    int modified;
    ReportNode *root;
} ReportTree;
static ReportNode *rep_liear_stack[30];
ReportTree *rep_init()
{
    ReportTree *t = malloc(sizeof(ReportTree));
    t->root = NULL;
    t->modified = 1;
    return t;
}
static ReportNode rep_sentinel = {0, 0, 0, &rep_sentinel, &rep_sentinel};
static inline void rep_leftRotation(ReportTree *tree, ReportNode *x)
{
    ReportNode *y = x->right;
    x->right = y->left;
    if (y->left != &rep_sentinel)
        y->left->parent = x;
    if (y != &rep_sentinel)
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
    if (x != &rep_sentinel)
        x->parent = y;
}
static inline void rep_rightRotation(ReportTree *tree, ReportNode *x)
{
    ReportNode *y = x->left;
    x->left = y->right;
    if (y->right != &rep_sentinel)
        y->right->parent = x;
    if (y != &rep_sentinel)
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
    if (x != &rep_sentinel)
        x->parent = y;
}
static inline void rep_insertFix(ReportTree *tree, ReportNode *x)
{
    ReportNode *y;
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
                    rep_leftRotation(tree, x);
                }
                x->parent->color = 0;
                x->parent->parent->color = 1;
                rep_rightRotation(tree, x->parent->parent);
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
                    rep_rightRotation(tree, x);
                }
                x->parent->color = 0;
                x->parent->parent->color = 1;
                rep_leftRotation(tree, x->parent->parent);
            }
        }
    }
    tree->root->color = 0;
}
ReportNode *rep_insert(ReportTree *tree, char *to, int *inserted)
{
    tree->modified = 1;

    int cmp = 0;
    ReportNode *parent = NULL, *itr = tree->root;
    while (itr && itr != &rep_sentinel)
    {
        cmp = strcmp(to, itr->data);
        if (cmp == 0)
        {
            *inserted = 0;
            itr->count++;
            return itr;
        }
        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;
    }
    ReportNode *node = malloc(sizeof(ReportNode));
    node->data = to;
    node->color = 1;
    node->left = &rep_sentinel;
    node->right = &rep_sentinel;
    node->parent = parent;
    node->count = 1; // start at 1 relation

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
    rep_insertFix(tree, node);
    *inserted = 1;
    return node;
}
ReportNode *rep_search(ReportTree *tree, char *to)
{
    ReportNode *itr = tree->root;
    while (itr && itr != &rep_sentinel)
    {
        int cmp = strcmp(to, itr->data);
        if (cmp == 0)
            break;
        else
            itr = (cmp > 0) ? itr->right : itr->left;
    }
    return itr != &rep_sentinel ? itr : NULL;
}
ReportNode *rep_treeMin(ReportNode *tree)
{
    while (tree->left != &rep_sentinel)
        tree = tree->left;
    return tree;
}
static inline void rep_deleteFix(ReportTree *tree, ReportNode *x)
{
    ReportNode *w;
    while (x != tree->root && x->color == 0)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            if (w->color == 1)
            {
                w->color = 0;
                x->parent->color = 1;
                rep_leftRotation(tree, x->parent);
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
                    rep_rightRotation(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->right->color = 0;
                rep_leftRotation(tree, x->parent);
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
                rep_rightRotation(tree, x->parent);
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
                    rep_leftRotation(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->left->color = 0;
                rep_rightRotation(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = 0;
}
void rep_delete(ReportTree *tree, ReportNode *z)
{
    if (!z)
        return;

    tree->modified = 1;

    ReportNode *x, *y;
    if (z->left == &rep_sentinel || z->right == &rep_sentinel)
    {
        y = z;
    }
    else
    {
        y = rep_treeMin(z->right);
    }
    if (y->left != &rep_sentinel)
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
        z->count = y->count;
    }
    if (y->color == 0)
        rep_deleteFix(tree, x);
    free(y);
    if (tree->root == &rep_sentinel)
        tree->root = NULL;
}
void rep_clean(ReportTree *tree)
{
    int used = 1;
    rep_liear_stack[0] = tree->root;
    if (!rep_liear_stack[0])
        return;
    ReportNode *p;
    while (used > 0)
    {
        p = rep_liear_stack[used - 1];
        used--;
        if (p->right != &rep_sentinel)
        {
            rep_liear_stack[used] = p->right;
            used++;
        }
        if (p->left != &rep_sentinel)
        {
            rep_liear_stack[used] = p->left;
            used++;
        }
        free(p);
    }
}
void rep_count(ReportTree *tree)
{
    int count = 0;
    int used = 1;
    rep_liear_stack[0] = tree->root;
    ReportNode *p;
    while (used > 0)
    {
        p = rep_liear_stack[used - 1];
        used--;
        if (p->right != &rep_sentinel)
        {
            rep_liear_stack[used] = p->right;
            used++;
        }
        if (p->left != &rep_sentinel)
        {
            rep_liear_stack[used] = p->left;
            used++;
        }
        count++;
    }
    printf("Tree elements: %d\n", count);
}



/**********************************************************
 * 
 * MAIN
 *  
 *********************************************************/

/****************************************
 * Delete relations
 ****************************************/

static RelationStorageData **rm_list = NULL;



static inline void remove_all_relations_for(EntityNode *ent, EntityTree *entities, RelationStorageTree *relations, ReportTree *reports[], int rep_count)
{
    static RelationStorageNode *stack[30];

    if(!relations->root)
        return; // no relations

    if(ent->relations > 0)
    {
        stack[0] = relations->root;
        RelationStorageNode *p;

        int stack_used = 1;

        int alloc_sz = 100;
        int used = 0;

        if (!rm_list)
            rm_list = malloc(100 * sizeof(RelationStorageData *));


        while (stack_used > 0) // stack not empty
        {
            p = stack[stack_used - 1];
            stack_used--;

            if (p->right != &rst_sentinel)
            {
                stack[stack_used] = p->right;
                stack_used++;
            }
            if (p->left != &rst_sentinel)
            {
                stack[stack_used] = p->left;
                stack_used++;
            }

            //add nodes to remove list
            if (p->data->from == ent->data || p->data->to == ent->data)
            {
                used++;
                if (used > alloc_sz)
                {
                    alloc_sz += 100;
                    rm_list = realloc(rm_list, alloc_sz * sizeof(RelationStorageData *));
                }

                rm_list[used - 1] = p->data;


                if (p->data->from == ent->data)
                {
                    ReportNode *rep = rep_search(reports[p->data->rel_id], p->data->to);
                    if (rep)
                    {
                        rep->count--;
                        reports[p->data->rel_id]->modified = 1;
                    }

                    EntityNode *dest = et_search(entities, p->data->to);
                    dest->relations--;

                }

            }

        }

        //delete relations
        for (int i = 0; i < used; i++)
        {
            rst_delete(relations, rm_list[i]->tree_node);
        }
    }

    //delete all reports
    for(int i = 0; i < rep_count; i++)
    {
        ReportNode *rep = rep_search(reports[i], ent->data);
        rep_delete(reports[i], rep);
    }


}


/****************************************
 * Report
 ****************************************/

static char *gb_report_cache[REPORT_TREES] = {0};



static inline int print_rep(char *rel, int rel_id, ReportTree *tree, int space)
{
    static int allocated[REPORT_TREES];
    static int last_used[REPORT_TREES];

    int out_last = 0;

    if(tree->modified)
    {

        static ReportNode *out[REPORT_OUT_QUEUE_SIZE];
        int max = 1;
        int used = 0;

        static ReportNode *stack[20];
        ReportNode *curr = tree->root;

        if (curr)
        {

            while (curr != &rep_sentinel || used > 0)
            {
                while (curr != &rep_sentinel)
                {
                    stack[used] = curr;
                    used++;
                    curr = curr->left;
                }

                curr = stack[used - 1];
                used--;

                //do shit
                //reset on greater
                if (curr->count > max)
                {
                    out[0] = curr;
                    max = curr->count;
                    out_last = 1;
                } else if (curr->count == max) // append on equal
                {
                    out[out_last] = curr;
                    out_last++;
                }

                curr = curr->right;

            }

        }

        #define GRCP gb_report_cache[rel_id]
        #define AP allocated[rel_id]
        #define LU last_used[rel_id]

        if(!GRCP)
        {
            GRCP = malloc(REPORT_BUFFER_SIZE);
            AP = REPORT_BUFFER_SIZE;
        }

        last_used[rel_id] = 0;

        if (out_last > 0)
        {
            int len = strlen(rel);

            if(LU + len > AP)
            {
                AP += REPORT_BUFFER_SIZE;
                GRCP = realloc(GRCP, AP);
            }

            memcpy(GRCP + LU, rel, len);
            LU += len;

            //printf("%s", rel);

            for (int i = 0; i < out_last; i++)
            {
                len = strlen(out[i]->data); // can optimize

                if(LU + len + 1> AP)
                {
                    AP += REPORT_BUFFER_SIZE;
                    GRCP = realloc(GRCP, AP);
                }

                GRCP[LU] = ' ';
                memcpy(GRCP + 1 + LU, out[i]->data, len);
                LU += len + 1;

                //printf(" %s", out[i]->data);

            }

            if(LU + 9 > AP)
            {
                AP += REPORT_BUFFER_SIZE;
                GRCP = realloc(GRCP, AP);
            }
            LU += sprintf(GRCP + LU, " %d;", max);
            //printf(" %d;", max);
        }

    }
    else
    {
        out_last = LU;
    }

    if(LU > 0)
    {
        if(space)
            fwrite(" ", 1, 1, stdout);

        fwrite(GRCP, 1, LU, stdout);

        //printf( space ? " %s" : "%s" , GRCP);
    }

    #undef GRCP
    #undef AP
    #undef LU

    tree->modified = 0;

    return out_last;
}


static inline void report(RelationNameTree *relNames, ReportTree *reports[])
{
    int used = 0;
    static RelationNameNode *stack[20];
    RelationNameNode *curr = relNames->root;

    int print = 0;

    if(curr)
    {

        while(curr != &rel_sentinel || used > 0)
        {
            while(curr != &rel_sentinel)
            {
                stack[used] = curr;
                used++;
                curr = curr->left;
            }

            curr = stack[used-1];
            used--;

            //do shit
            if(reports[curr->id] && reports[curr->id]->root)
            {
                int res = print_rep(curr->data, curr->id, reports[curr->id], print);

                if(res > 0) print = 1;
            }

            curr = curr->right;
        }
    }

    if(!print)
        fwrite("none\n", 1, 5, stdout);
    else
        fwrite("\n", 1, 1, stdout);

}


/****************************************
 * MAIN
 ****************************************/

int main(int argc, char** argv)
{

    EntityTree *entities = et_init();
    RelationNameTree *relationNames = rel_init();
    RelationStorageTree *relations = rst_init();

    ReportTree *reports[REPORT_TREES] = {0};

    #ifdef DEBUG
    double start_tm = ns();
    #endif

    //init
    char *command[3];
    char *buffer = malloc(sizeof(char) * INPUT_BUFFER_SIZE);

    int exit_loop = 0;


    #ifdef DEBUG
    FILE *fl = fopen("test.txt","r");
    if(!fl) fl= stdin;
    #else
    FILE *fl = stdin;
    #endif
    
    /*
     *   INPUT
     */

    do
    {   
        size_t max_sz = INPUT_BUFFER_SIZE;
        size_t rsz = getline(&buffer, &max_sz, fl);

        if(buffer[0] == 'a')
        {
            if(buffer[3] == 'e')
            {
                //addent <ent>
                command[0] = malloc(rsz-7);
                memcpy(command[0], (buffer + 7), rsz-8);
                command[0][rsz-8] = '\0';

                int res;
                et_insert(entities, command[0], &res);
                if(!res)
                {
                    free(command[0]);
                }

            }
            else if(buffer[3] == 'r')
            {
                //addrel <from> <to> <rel>

                int spaces = 0;
                int last_space = 6; //position of the first space
                for(int i = 7; i < rsz && spaces < 2; i++)
                {
                    if(buffer[i] == ' ')
                    {
                        command[spaces] =  malloc(i-last_space);
                        memcpy(command[spaces], buffer + last_space + 1, i - last_space -1 );
                        command[spaces][i-last_space-1] = '\0';
                        last_space = i;
                        spaces++;
                    }
                }
                command[2] =  malloc(rsz - last_space-1);
                memcpy(command[2], (buffer + last_space+1), rsz-last_space-2);
                command[2][rsz-last_space-2] = '\0';

                // do insertion if possibile
                int res = 0;
                EntityNode *source = et_search(entities, command[0]);
                if(source)
                {
                    EntityNode *dest = et_search(entities, command[1]);
                    if(dest)
                    {

                        RelationNameNode *rel =  rel_insert(relationNames, command[2], &res);

                        int r2 = 0;
                        rst_insert(relations, source->data, dest->data, rel->data, rel->id, &r2);


                        if(r2)
                        {
                            source->relations++;
                            dest->relations++;

                            if (!reports[rel->id])
                            {
                                reports[rel->id] = rep_init();
                            }

                            rep_insert(reports[rel->id], dest->data, &r2);
                        }
                    }
                }

                free(command[0]);
                free(command[1]);
                if(!res)
                    free(command[2]);

            }
        }
        else if(buffer[0] == 'd')
        {
            if(buffer[3] == 'e')
            {
                //delent <ent>
                command[0] = malloc(rsz-7);
                memcpy(command[0], (buffer + 7), rsz-8);
                command[0][rsz-8] = '\0';

                EntityNode *res = et_search(entities, command[0]);
                if(res)
                {
                    remove_all_relations_for(res, entities, relations, reports, relationNames->count);
                    et_delete(entities, res);
                }
                free(command[0]);
                

            }
            else if(buffer[3] == 'r')
            {
                //delrel <from> <to> <rel>
                int spaces = 0;
                int last_space = 6; //position of the first space
                for(int i = 7; i < rsz && spaces < 2; i++)
                {
                    if(buffer[i] == ' ')
                    {
                        command[spaces] =  malloc(i-last_space);
                        memcpy(command[spaces], buffer + last_space + 1, i - last_space -1 );
                        command[spaces][i-last_space-1] = '\0';
                        last_space = i;
                        spaces++;
                    }
                }
                command[2] =  malloc(rsz - last_space-1);
                memcpy(command[2], (buffer + last_space+1), rsz-last_space-2);
                command[2][rsz-last_space-2] = '\0';

                RelationStorageNode *del = rst_search(relations, command[0], command[1], command[2]);
                if(del)
                {
                    int rel_id = rst_delete(relations, del);

                    EntityNode* dest =  et_search(entities, command[1]);
                    EntityNode* source = et_search(entities, command[0]);

                    source->relations--;
                    dest->relations--;

                    ReportNode *rep = rep_search(reports[rel_id], command[1]);
                    if(rep)
                    {
                        rep->count--;
                        reports[rel_id]->modified = 1;
                    }
                    else
                    {
                        DEBUG_PRINT("RM Error: unable to find report data for %s\n", command[1]);
                    }
                }

                free(command[0]);
                free(command[1]);
                free(command[2]);

            }
        }
        else if(buffer[0] == 'r')
        {
            //report
            report(relationNames, reports);
        }
        else
        {
            //end 
            exit_loop = 1;
        }

    } while (!exit_loop);

    //rm buffer
    free(buffer);

    et_clean(entities);
    free(entities);

    rel_clean(relationNames);
    free(relationNames);

    rst_clean(relations);
    free(relations);

    for(int i = 0; i <REPORT_TREES; i++)
    {
        if(reports[i])
        {
            rep_clean(reports[i]);
            free(reports[i]);
        }
        if(gb_report_cache[i])
            free(gb_report_cache[i]);
    }


    if(rm_list)
        free(rm_list);

    #ifdef DEBUG
    if(fl) fclose(fl);

    double msTm = (ns() - start_tm)/1000000;
    printf("\nExecution time: %.2fms ~ %.2fs (%.2fns)\n", msTm, msTm/1000, ns() - start_tm);
    #endif

    return 0;
}