/*
    Have fun reading this hell :3
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

#define INPUT_BUFFER_SIZE 1024
#define REPORT_ELEMENTS_SIZE 512
#define REPORT_BUFFER_SIZE 512


#ifdef DEBUG
#define SUPPORTED_RELATIONS 32
#else
#define SUPPORTED_RELATIONS 4
#endif

#ifdef DEBUG
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT(...)
#endif

#ifdef DEBUG
#include "bench.c"
#endif

#ifdef DEBUG
#define FORCE_INLINE __attribute__((always_inline)) inline
#else
#define FORCE_INLINE inline
#endif



/* MAIN ONLY MACROS */

#define GRAB_CMD_0                                                                  \
command[0] = malloc(rsz-7);                                                         \
memcpy(command[0], (buffer + 7), rsz-8);                                            \
command[0][rsz-8] = '\0';


#define GRAB_CMD_0_1_2                                                              \
int spaces = 0;                                                                     \
int last_space = 6;                                                                 \
for(int i = 7; i < rsz && spaces < 2; i++)                                          \
{                                                                                   \
    if(buffer[i] == ' ')                                                            \
    {                                                                               \
        command[spaces] =  malloc(i-last_space);                                    \
        memcpy(command[spaces], buffer + last_space + 1, i - last_space -1 );       \
        command[spaces][i-last_space-1] = '\0';                                     \
        last_space = i;                                                             \
        spaces++;                                                                   \
    }                                                                               \
}                                                                                   \
command[2] =  malloc(rsz - last_space-1);                                           \
memcpy(command[2], (buffer + last_space+1), rsz-last_space-2);                      \
command[2][rsz-last_space-2] = '\0';

/* END MAIN ONLY MACROS END */


struct s_rcontainer;

typedef struct
{
    char *name;
    struct s_rcontainer *rel_container;

} EntityData;


typedef struct s_EntityNode
{
    EntityData *data;
    char color;
    struct s_EntityNode *parent, *right, *left;
} EntityNode;


typedef struct
{
    EntityNode *root;
} EntityTree;




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
        cmp = strcmp(entity, itr->data->name);
        if (cmp == 0)
        {
            *inserted = 0;
            return itr;
        }
        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;
    }
    EntityNode *node = malloc(sizeof(EntityNode));

    node->data = malloc(sizeof(EntityData));
    memset(node->data, 0, sizeof(EntityData));
    node->data->name = entity;
    node->data->rel_container = NULL;

    node->color = 1;
    node->left = &et_sentinel;
    node->right = &et_sentinel;
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

    et_insertFix(tree, node);

    *inserted = 1;
    return node;
}


EntityNode *et_search(EntityTree *tree, char *entity)
{
    EntityNode *itr = tree->root;

    while (itr && itr != &et_sentinel)
    {
        int cmp = strcmp(entity, itr->data->name);
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


void rc_clean(EntityData *ent); // fwd def


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
        EntityData *temp = z->data;
        z->data = y->data;
        y->data = temp;
    }
    if (y->color == 0)
        et_deleteFix(tree, x);

    free(y->data->name);

    if(y->data->rel_container)
        rc_clean(y->data);

    free(y->data);
    free(y);

    if (tree->root == &et_sentinel)
        tree->root = NULL;
}


void et_clean(EntityTree *tree)
{
    int used = 1;

    EntityNode *et_liear_stack[30];

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

        if(p->data->rel_container)
            rc_clean(p->data);

        free(p->data->name);
        free(p->data);
        free(p);
    }
}




typedef struct s_ReportNode
{
    char *data;
    short count;
    char color;
    struct s_ReportNode *parent, *right, *left;
} ReportNode;


typedef struct
{
    int modified;
    short max;
    ReportNode *root;
} ReportTree;


static ReportNode *rep_liear_stack[30];


ReportTree *rep_init()
{
    ReportTree *t = malloc(sizeof(ReportTree));
    t->root = NULL;
    t->modified = 1;
    t->max = 0;
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

    int cmp = 0;
    ReportNode *parent = NULL, *itr = tree->root;
    while (itr && itr != &rep_sentinel)
    {
        cmp = strcmp(to, itr->data);
        if (cmp == 0)
        {
            if(inserted)
                *inserted = 0;

            itr->count++;

            if(itr->count >= tree->max)
                tree->modified = 1;

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

    if(1 >= tree->max)
        tree->modified = 1;

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

    if(inserted)
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

    if(z->count >= tree->max)
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


/**
 * Decrease count by one, if count reach 0 delete element
 * @param tree
 * @param to
 * @return 1 if element is deleted
 */
int rep_decrease(ReportTree *tree, char *to)
{
    ReportNode *rep = rep_search(tree,to);
    if(!rep)
        return 0;

    if(rep->count >= tree->max)
        tree->modified = 1;

    rep->count--;

    if(rep->count == 0)
    {
        rep_delete(tree, rep);
    }

    return 1;
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




ReportTree reports[SUPPORTED_RELATIONS]; /* ITS EPIC COLABRODO TIME */



#define REL_NOT_FOUND -1

typedef char RelationID;

typedef struct
{
    char *name;
    RelationID id;

} RelationType ;


typedef struct
{
    RelationType rels[SUPPORTED_RELATIONS];
    RelationID lastID; //also used as size
} RelationTypeManager;


/****************************************
 *
 *  CODE
 *
 ****************************************/

/**
 * Init rtm
 * @return
 */
RelationTypeManager *rtm_init()
{
    RelationTypeManager *rtm = malloc(sizeof(RelationTypeManager));
    memset(rtm, 0, sizeof(RelationTypeManager));
    return rtm;
}

/**
 * Search
 * @param rtm
 * @param relName
 * @return
 */
int rtm_searchIDX(RelationTypeManager *rtm, char *relName)
{
    int low =0, height = rtm->lastID -1, mid, cmp;

    while(low <= height)
    {
        mid = (low + height)/2;

        cmp = strcmp(relName, rtm->rels[mid].name);

        if(cmp == 0)
        {
            return mid;
        }
        else if(cmp < 0)
        {
            height = mid - 1;
        }
        else
        {
            low = mid + 1;
        }
    }

    return REL_NOT_FOUND;
}

/**
 * Search wreapper
 * @param rtm
 * @param relName
 * @return
 */
RelationType *rtm_search(RelationTypeManager *rtm, char *relName)
{
    int sch = rtm_searchIDX(rtm, relName);

    return sch == REL_NOT_FOUND ? NULL : &rtm->rels[sch];
}


/**
 * Insert with duplicate check (if exist return old ptr)
 * @param rtm
 * @param relName
 * @return
 */
RelationType *rtm_insert(RelationTypeManager *rtm, char *relName, int *inserted)
{
    //check duplicate
    RelationType *dup = rtm_search(rtm, relName);
    if(dup)
    {
        *inserted = 0;
        return dup;
    }

    int i;
    for(i = rtm->lastID - 1; i >= 0 && strcmp(relName, rtm->rels[i].name) < 0; i--)
    {
        rtm->rels[i+1].name = rtm->rels[i].name;
        rtm->rels[i+1].id = rtm->rels[i].id;
    }

    rtm->rels[i+1].name = relName;
    rtm->rels[i+1].id = rtm->lastID++;

    *inserted = 1;
    return &rtm->rels[i+1];
}


/**
 * Delete everything
 */
void rtm_clean(RelationTypeManager *rtm)
{
    for (int i = 0; i < rtm->lastID; i++)
    {
        free(rtm->rels[i].name);
    }

    free(rtm);
}



typedef char RelationID;


typedef struct
{
    EntityData **rels[SUPPORTED_RELATIONS];
    short sizes[SUPPORTED_RELATIONS];
    short allocated[SUPPORTED_RELATIONS];
} RelationHolder;


/****************************************
 *
 *  CODE
 *
 ****************************************/

/**
 * Init rtm
 * @return
 */
void rh_init(RelationHolder *rh)
{
    memset(rh, 0, sizeof(RelationHolder));
}

/**
 * Search
 * @param rtm
 * @param relName
 * @return
 */
int rh_searchIDX(RelationHolder *rh, char *other, RelationID relID)
{
    int low =0, height = rh->sizes[relID] -1, mid, cmp;

    while(low <= height)
    {
        mid = (low + height)/2;

        cmp = strcmp(other, rh->rels[relID][mid]->name);

        if(cmp == 0)
        {
            return mid;
        }
        else if(cmp < 0)
        {
            height = mid - 1;
        }
        else
        {
            low = mid + 1;
        }
    }

    return REL_NOT_FOUND;
}

/**
 * Search wreapper
 * @param rtm
 * @param relName
 * @return
 */
EntityData *rh_search(RelationHolder *rh, char *other, RelationID relID)
{
    int sch = rh_searchIDX(rh, other, relID);

    return sch == REL_NOT_FOUND ? NULL : rh->rels[relID][sch];
}


/**
 * Insert with duplicate check (if exist return old ptr)
 * @param rtm
 * @param relName
 * @return 1 if insered new, 0 if duplicate
 */
int rh_insert(RelationHolder *rh, EntityData *other, RelationID relID)
{
    //check duplicate
    EntityData *dup = rh_search(rh, other->name, relID);
    if(dup)
    {
        return 0;
    }

    //check alloc
    if(rh->sizes[relID] + 1 > rh->allocated[relID])
    {
        rh->allocated[relID] += 10;
        rh->rels[relID] = realloc(rh->rels[relID], rh->allocated[relID] * sizeof(EntityData *));
    }

    //insert in order
    int i;
    for(i = rh->sizes[relID] - 1; i >= 0 && strcmp(other->name, rh->rels[relID][i]->name) < 0; i--)
    {
        rh->rels[relID][i+1] = rh->rels[relID][i];
    }

    rh->rels[relID][i+1] = other;

    rh->sizes[relID]++;

    return 1;
}


/**
 * delete relation
 * @param rtm
 * @param relName
 */
EntityData *rh_remove(RelationHolder *rh, char *other, RelationID relID)
{
    int start = rh_searchIDX(rh, other, relID);

    if(start == REL_NOT_FOUND)
        return NULL;

    EntityData *ent = rh->rels[relID][start];

    //delete in order
    for(; start < rh->sizes[relID] - 1; start++)
    {
        rh->rels[relID][start] = rh->rels[relID][start + 1];
    }

    rh->sizes[relID]--;

    //nothing to free ent datas are free by tree
    return ent;
}


/**
 * Delete everything
 */
void rh_clean(RelationHolder *rh)
{
    for (int i = 0; i < SUPPORTED_RELATIONS; i++)
    {
        if(rh->rels[i])
            free(rh->rels[i]);
    }
}


typedef struct s_rcontainer
{
    RelationHolder in;
    RelationHolder out;

} RelationContainer;


/**
 * Assign container if not allocated
 * @param ent
 */
void rc_assign_relation_container(EntityData *ent)
{
    if(ent->rel_container)
        return;
    ent->rel_container = malloc(sizeof(RelationContainer));
    memset(ent->rel_container, 0, sizeof(RelationContainer));
}



int rc_make_relation(EntityData *from, EntityData* to, RelationID relID)
{
    rc_assign_relation_container(from);
    rc_assign_relation_container(to);

    if(rh_insert(&from->rel_container->out, to, relID))
    {
        rh_insert(&to->rel_container->in, from, relID);

        //update cache
        rep_insert(&reports[relID], to->name, NULL);

        return 1; // inserted new rel
    }

    return 0; // duplicate

}

int rc_delete_relation(EntityData *from, char *to, RelationID relID)
{
    if(!from->rel_container)
        return 0;

    EntityData *del = rh_remove(&from->rel_container->out, to, relID);
    if(del)
    {
        //rel exists
        rh_remove(&del->rel_container->in, from->name, relID);

        rep_decrease(&reports[relID], to);

        return 1;
    }

    return 0;
}


/**
 * Delete all relations for an entity (this also clears internal items)
 * @param ent
 */
void rc_delete_all_for(EntityData *ent)
{
    if(!ent->rel_container)
        return;

    //remove in rels (of current)
    for(int i = 0; i < SUPPORTED_RELATIONS; i++)
    {
        EntityData **arr = ent->rel_container->in.rels[i];

        for(int j =0; j < ent->rel_container->in.sizes[i]; j++)
        {
            rh_remove(&arr[j]->rel_container->out, ent->name, i);
        }

        if(arr)
        {
            free(arr);
            //also remove from reports
            rep_delete(&reports[i], rep_search(&reports[i], ent->name));

        }

    }

    //remove out rels
    for(int i = 0; i < SUPPORTED_RELATIONS; i++)
    {
        EntityData **arr = ent->rel_container->out.rels[i];

        for(int j =0; j < ent->rel_container->out.sizes[i]; j++)
        {
            rh_remove(&arr[j]->rel_container->in, ent->name, i);

            rep_decrease(&reports[i], arr[j]->name);
        }

        if(arr)
            free(arr);
    }

    free(ent->rel_container);
    ent->rel_container = NULL;

    //skip remove of current object items to speed up things :#
}


void rc_clean(EntityData *ent)
{
    rh_clean(&ent->rel_container->in);
    rh_clean(&ent->rel_container->out);

    free(ent->rel_container);
    ent->rel_container = NULL;
}



/****************************************
 * MAIN
 ****************************************/


static char *gb_report_cache[SUPPORTED_RELATIONS] = {0};



static inline int print_rep(char *rel, int rel_id, ReportTree *tree, int space)
{
    static int allocated[SUPPORTED_RELATIONS];
    static int last_used[SUPPORTED_RELATIONS];

    int out_last = 0;

    if(tree->modified)
    {

        static ReportNode *out[REPORT_ELEMENTS_SIZE];
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
            }

            tree->max = max;

            if(LU + 9 > AP)
            {
                AP += REPORT_BUFFER_SIZE;
                GRCP = realloc(GRCP, AP);
            }
            LU += sprintf(GRCP + LU, " %d;", max);
        }
        else
        {
            tree->max = 0;
        }

    }
    else
    {
        out_last = LU;
    }

    if(LU > 0)
    {
        if(space)
            fputs(" ", stdout);

        fwrite(GRCP,1, LU, stdout);

    }

    #undef GRCP
    #undef AP
    #undef LU

    tree->modified = 0;

    return out_last;
}

void report(RelationTypeManager *rtm)
{
    short print = 0;
    RelationID id;

    for(int i =0 ; i < rtm->lastID; i++)
    {
        id = rtm->rels[i].id;
        print += print_rep(rtm->rels[i].name, id, &reports[id], print);
    }

    if(!print)
        fputs("none\n", stdout);
    else
        fputs("\n", stdout);

}

/****************************************
 * MAIN
 ****************************************/

int main(int argc, char** argv)
{

    EntityTree *entities = et_init();
    RelationTypeManager *rtm = rtm_init();

    #ifdef DEBUG
    double start_tm = ns();

    static int LINE = 0;
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
        #ifdef DEBUG
        LINE++;
        #endif

        size_t max_sz = INPUT_BUFFER_SIZE;
        size_t rsz = getline(&buffer, &max_sz, fl);

        if(buffer[0] == 'a')
        {
            if(buffer[3] == 'e')
            {
                //addent <ent>
                GRAB_CMD_0

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

                GRAB_CMD_0_1_2

                // do insertion if possibile
                int res = 0;
                EntityNode *source = et_search(entities, command[0]);
                if(source)
                {
                    EntityNode *dest = et_search(entities, command[1]);
                    if(dest)
                    {
                        RelationType *rel = rtm_insert(rtm, command[2], &res);

                        rc_make_relation(source->data, dest->data, rel->id);

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
                GRAB_CMD_0

                EntityNode *res = et_search(entities, command[0]);
                if(res)
                {
                    rc_delete_all_for(res->data);
                    et_delete(entities, res);
                }

                free(command[0]);


            }
            else if(buffer[3] == 'r')
            {
                //delrel <from> <to> <rel>
                GRAB_CMD_0_1_2

                RelationType *rel = rtm_search(rtm, command[2]);
                if(rel)
                {
                    EntityNode *src = et_search(entities, command[0]);

                    if(src)
                    {
                        rc_delete_relation(src->data, command[1], rel->id);
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
            report(rtm);

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

    rtm_clean(rtm);

    for(int i =0; i < SUPPORTED_RELATIONS; i++)
    {
        rep_clean(&reports[i]);
        if(gb_report_cache[i])
            free(gb_report_cache[i]);
    }

    #ifdef DEBUG
    if(fl) fclose(fl);

    double msTm = (ns() - start_tm)/1000000;
    printf("\nExecution time: %.2fms ~ %.2fs (%.2fns)\n", msTm, msTm/1000, ns() - start_tm);
    #endif

    return 0;
}