
#include <stdlib.h>

static inline char *rel_allocate(char *data) { return data; }
static inline int rel_compare(char *x, char *y) { return strcmp(y, x); }
static inline void rel_deallocate(char *data) { free(data); }

#define RANK_INCEMENT 64

typedef struct s_RelationNameNode
{
    char *data;
    int color;
    int id;

    ReportLink **ranks;
    int best_rank;
    int allocated_ranks;

    struct s_RelationNameNode *parent, *right, *left;
} RelationNameNode;

typedef struct
{
    int last_insert;
    RelationNameNode *root;
} RelationNameTree;

static RelationNameNode *rel_liear_stack[30];
RelationNameTree *rel_init()
{
    RelationNameTree *t = malloc(sizeof(RelationNameTree));
    t->root = NULL;
    t->last_insert = 0;
    return t;
}

static RelationNameNode rel_sentinel = {0, 0, 0, 0, 0, 0, 0, 0, &rel_sentinel, &rel_sentinel};

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
        cmp = rel_compare(itr->data, relation);
        if (cmp == 0)
        {
            *inserted = 0;
            return itr;
        }
        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;
    }
    RelationNameNode *node = malloc(sizeof(RelationNameNode));
    node->data = rel_allocate(relation);
    node->color = 1;
    node->left = &rel_sentinel;
    node->right = &rel_sentinel;
    node->parent = parent;
    node->id = tree->last_insert;

    //we love eating ram so pre-allocate :3
    node->ranks = (ReportLink **)malloc(RANK_INCEMENT * sizeof(ReportLink*) );
    node->allocated_ranks = RANK_INCEMENT;
    node->best_rank = 0;
    memset(node->ranks, 0 , RANK_INCEMENT * sizeof(ReportLink*) );

    tree->last_insert+=1;

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
        z->id = y->id;
        y->data = temp;
        z->allocated_ranks = y->allocated_ranks;
        z->best_rank = y->best_rank;
        ReportLink **t = z->ranks;
        z->ranks = y->ranks;
        y->ranks = t;
    }
    if (y->color == 0)
        rel_deleteFix(tree, x);

    free(y->ranks);
    rel_deallocate(y->data);
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

        //free all ranks
        for(int i =0; i < p->best_rank; i++)
        {
            ReportLink *del, *itr = p->ranks[i];
            while(itr)
            {
                del = itr;
                itr = itr->next;
                free(del);
            }
        }

        free(p->ranks);
        rel_deallocate(p->data);
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
    printf("Tree elements: %d", count);
}


ReportLink *rel_make_rank(RelationNameNode* rel, char* who)
{
    ReportLink *link = malloc(sizeof(ReportLink));
    link->to = who;
    link->count = 1;

    //insert into ranks
    link->next = rel->ranks[1];
    link->prev = NULL;

    if(rel->ranks[1])
        rel->ranks[1]->prev = link;

    rel->ranks[1] = link;

    if(rel->best_rank < 1) rel->best_rank = 1;

    return link;
}

void rel_update_rank(RelationNameNode* rel, ReportLink *link, int dir)
{

    //remove link from ranks
    if(!link->prev)
    {
        rel->ranks[link->count] = link->next;
    }
    else
    {
        link->prev->next = link->next;
    }

    if(link->next) link->next->prev = link->prev;

    link->count += (dir > 0) ? 1 : -1;

    if(link->count >= rel->allocated_ranks)
    {
        rel->allocated_ranks += RANK_INCEMENT;
        rel->ranks = realloc(rel->ranks, sizeof(ReportLink *) * rel->allocated_ranks);
        memset(rel->ranks + rel->allocated_ranks - RANK_INCEMENT, 0, RANK_INCEMENT * sizeof(ReportLink *)); // set to 0 new nodes
    }

    //insert into ranks
    link->next = rel->ranks[link->count];
    link->prev = NULL;

    if(rel->ranks[link->count])
        rel->ranks[link->count]->prev = link;

    rel->ranks[link->count] = link;

    if(rel->best_rank < link->count) rel->best_rank = link->count;
}

/**
 * Delete link, this DOES NOT update best rank
 * its an expesinve call so it must be done with care
 * @param rel
 * @param link
 */
void rel_delete_rank(RelationNameNode* rel, ReportLink *link)
{

    //remove link from ranks
    if(!link->prev)
    {
        rel->ranks[link->count] = link->next;
    }
    else
    {
        link->prev->next = link->next;
    }

    if(link->next) link->next->prev = link->prev;

    free(link);
}

/**
 * Recalculate best rank for a relation
 * @param rel
 */
void rel_recompute_best_rank(RelationNameNode* rel)
{
    int val = rel->best_rank;
    for(; val >= 0; val--)
    {
        if(rel->ranks[val])
            break;
    }

    rel->best_rank = val;
}