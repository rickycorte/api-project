#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEBUG

#define INPUT_BUFFER_SIZE 1024

#define RELATION_INCREMENT 10

#ifdef DEBUG
    #define SUPPORTED_RELATION_TYPES 33
#else
    #define  SUPPORTED_RELATION_TYPES 5
#endif

#ifdef DEBUG
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT(...)
#endif

#ifdef DEBUG
    #include "bench.c"
#endif


/**************************************************************
 *
 *
 * DATA STRUCTURES
 *
 *
 **************************************************************/

typedef unsigned char uu_short;


//fwd declarations
typedef struct s_relation RelationData;

/* relations:
 *      - incoming : current entity is the relation "from" field
 *      - outgoing : current entity is the relation "to" field
 *
 */


//declarations

typedef struct
{
    char *name;
    unsigned short incoming_rel_count[SUPPORTED_RELATION_TYPES]; // count of incoming relations (approx 2,5 ints on release)
    RelationData** relations; // list of relations of this entity (both from and to)
    unsigned short relations_size;
    unsigned short relations_allocated;
} EntityData;


struct s_relation
{
    EntityData *from;
    EntityData *to;
    uu_short relID;
    int source_idx, dest_idx; // cache index to reset
    void *node_ptr; // pointer to rbtree node containing this item
};




/**************************************************************
 *
 *
 * ENTITY TREE
 *
 *
 **************************************************************/


typedef struct s_EntityNode
{
    EntityData *data;
    uu_short color;
    struct s_EntityNode *parent, *right, *left;
} EntityNode;


typedef struct
{
    EntityNode *root;
} EntityTree;



static EntityNode et_sentinel = {0, 0, 0, &et_sentinel, &et_sentinel};


/**************************************************************
 * INIT
 **************************************************************/


/**
 * Initialize a new empty entity tree
 * @return
 */
EntityTree *et_init()
{
    EntityTree *t = malloc(sizeof(EntityTree));
    t->root = NULL;
    return t;
}


/**************************************************************
 * ROTATIONS
 **************************************************************/

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

/**************************************************************
 * INSERT
 **************************************************************/

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

/**
 * Insert a new node in the tree (w/ duplicate check)
 * @param tree
 * @param name entity name to insert
 * @param inserted 1 if inserted new element, 0 duplicate found
 * @return new node pointer/ pointer to just existent node
 */
EntityNode *et_insert(EntityTree *tree, char *name, int *inserted)
{
    int cmp = 0;
    EntityNode *parent = NULL, *itr = tree->root;

    //search insert pos / duplicate check
    while (itr && itr != &et_sentinel)
    {
        cmp = strcmp(name, itr->data->name);
        if (cmp == 0)
        {
            *inserted = 0;
            return itr;
        }
        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;
    }

    //allocate new node
    EntityNode *node = malloc(sizeof(EntityNode));
    node->data = malloc(sizeof(EntityData));

    memset(node->data, 0, sizeof(EntityData));
    node->data->name = name;

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

/**************************************************************
 * SEARCH
 **************************************************************/

/**
 * Search if a entity is in the tree
 *
 * @param tree
 * @param entity entity to search
 * @return pointer to found entity, NULL if not found
 */
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


/**************************************************************
 * DELETE
 **************************************************************/

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

/**
 * Delete a entity in the tree (if passed entity is null this function exits)
 * @param tree
 * @param z pointer to entity to delete
 */
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
    if(y->data->relations)
        free(y->data->relations);
    //relations items should be freed in by others

    free(y->data);
    free(y);

    //fix root
    if (tree->root == &et_sentinel)
        tree->root = NULL;
}


/**************************************************************
 * CLEANUP
 **************************************************************/

/**
 * Delete all tree elements (delets tree instance too)
 * @param tree
 */
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

        free(p->data->name);
        if(p->data->relations)
            free(p->data->relations);
        //relations items should be freed in by others

        free(p->data);
        free(p);
    }

    free(tree);
}

/**************************************************************
 * MISC
 **************************************************************/

void et_count(EntityTree *tree)
{
    int count = 0;
    int used = 1;
    EntityNode *et_liear_stack[30];
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



/**************************************************************
 *
 *
 * RELATION TREE
 *
 *
 **************************************************************/

typedef struct s_RelationStorageNode
{
    RelationData *data;
    uu_short color;
    struct s_RelationStorageNode *parent, *right, *left;
} RelationStorageNode;


typedef struct
{
    RelationStorageNode *root;
} RelationStorageTree;


static RelationStorageNode rst_sentinel = {0, 0, 0, &rst_sentinel, &rst_sentinel};


/**************************************************************
 * COMPARATOR
 **************************************************************/


static inline int rst_compare(RelationData *x, char *from, char *to)
{
    int res = strcmp(from, x->from->name);
    if(res) return res;

    return strcmp(to, x->to->name);
}



/**************************************************************
 * INIT
 **************************************************************/

/**
 * Initialize a new tree
 * @return
 */
RelationStorageTree *rst_init()
{
    RelationStorageTree *t = malloc(sizeof(RelationStorageTree));
    t->root = NULL;
    return t;
}


/**************************************************************
 * ROTATIONS
 **************************************************************/

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


/**************************************************************
 * INSERTION
 **************************************************************/

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

/**
 * Insert a new relation in the tree (w/ duplicate check)
 * @param tree
 * @param from
 * @param to
 * @param inserted
 * @return
 */
RelationStorageNode *rst_insert(RelationStorageTree *tree, EntityData *from, EntityData *to, uu_short relID, int *inserted)
{
    int cmp = 0;
    RelationStorageNode *parent = NULL, *itr = tree->root;
    while (itr && itr != &rst_sentinel)
    {
        cmp = rst_compare(itr->data, from->name, to->name);
        if (cmp == 0)
        {
            *inserted = 0;
            return itr;
        }
        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;
    }

    //allocate new node
    RelationStorageNode *node = malloc(sizeof(RelationStorageNode));
    node->data = malloc(sizeof(RelationData));
    node->data->from = from;
    node->data->to = to;
    node->data->node_ptr = node;
    node->data->relID = relID;

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


/**************************************************************
 * SEARCH
 **************************************************************/

/**
 * Search item in the tree
 * @param tree
 * @param from
 * @param to
 * @param rel
 * @return
 */
RelationStorageNode *rst_search(RelationStorageTree *tree, char *from, char *to)
{
    RelationStorageNode *itr = tree->root;
    while (itr && itr != &rst_sentinel)
    {
        int cmp = rst_compare(itr->data, from, to);
        if (cmp == 0)
            break;
        else
            itr = (cmp > 0) ? itr->right : itr->left;
    }
    return itr != &rst_sentinel ? itr : NULL;
}

/**************************************************************
 * DELETE
 **************************************************************/

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
        RelationData *temp = z->data;
        z->data = y->data;
        y->data = temp;
        z->data->node_ptr = z; // assign new address of node

    }
    if (y->color == 0)
        rst_deleteFix(tree, x);

    free(y->data);
    free(y);

    if(tree->root == &rst_sentinel)
        tree->root = NULL;
}

/**************************************************************
 * CLEANUP
 **************************************************************/

/**
 * Delete all elements in tree
 * @param tree
 */
void rst_clean(RelationStorageTree *tree)
{
    int used = 1;
    RelationStorageNode *rst_liear_stack[30];

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

        free(p->data);
        free(p);
    }
}

/**************************************************************
 *
 *
 * RELATION TYPES
 *
 *
 **************************************************************/

// global is trash and wrong but why not :3

char *relation_types[SUPPORTED_RELATION_TYPES] = {0};

uu_short ordered_relations_indexes[SUPPORTED_RELATION_TYPES]; // contain index of relation_types in alphabetical order

RelationStorageTree relation_trees[SUPPORTED_RELATION_TYPES] = {0}; // init trees to 0 (same as rst_init )

uu_short last_rel_type_id = 0;


// code here is optimized for release only, sucks for more generic tests

/**
 * Insert a new relation on return the REAL index (insert order)
 * @param rel_name
 * @return
 */
uu_short rel_id_insert(char *rel_name)
{
    relation_types[last_rel_type_id] = rel_name;

    //insert index in ordered array
    uu_short i = 0;
    for(i = last_rel_type_id - 1;
        i >=0 && strcmp(relation_types[ordered_relations_indexes[i]], relation_types[ordered_relations_indexes[i+1]]) < 0;
        i--)
    {
        ordered_relations_indexes[i+1] = ordered_relations_indexes[i];
    }
    ordered_relations_indexes[i+1] = last_rel_type_id;

    last_rel_type_id++;

    return last_rel_type_id-1;
}

/**
 * Search a relation
 * @param rel_name
 * @return REAL index of relation, ( a number greater than max supported relations in not found)
 */
uu_short rel_id_search(char *rel_name)
{
    for(int i=0; i < last_rel_type_id; i++)
    {
        if(strcmp(rel_name, relation_types[i]) == 0)
            return i;
    }

    //for just a few elements its as good as binary search but much easier :3

    return SUPPORTED_RELATION_TYPES + 1;
}


/**************************************************************
 *
 *
 * CORE
 *
 *
 **************************************************************/

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


//unoptimized as hell
void report(EntityTree *entities)
{
    int used = 1;
    static EntityNode *stack[30];

    unsigned short maxes[SUPPORTED_RELATION_TYPES] = {0};

    stack[0] = entities->root;
    if (!stack[0])
    {
        fputs("none\n", stdout);
        return;
    }

    //find max
    EntityNode *p;
    while (used > 0)
    {
        p = stack[used - 1];
        used--;
        if (p->right != &et_sentinel)
        {
            stack[used] = p->right;
            used++;
        }
        if (p->left != &et_sentinel)
        {
            stack[used] = p->left;
            used++;
        }

        for(int i = 0; i < last_rel_type_id; i++)
        {
            if(p->data->incoming_rel_count[i] > maxes[i])
            {
                maxes[i] = p->data->incoming_rel_count[i];
            }
        }
    }

    int print = 0;

    //print slowly as hell (4x n)
    for(int i =0; i < last_rel_type_id; i++)
    {
        if(maxes[i] == 0)
            continue;

        if(print)
            fputs(" ",stdout);
        fputs(relation_types[ordered_relations_indexes[i]], stdout);

        used = 1;
        stack[0] = entities->root;
        while (used > 0)
        {
            p = stack[used - 1];
            used--;
            if (p->right != &et_sentinel)
            {
                stack[used] = p->right;
                used++;
            }
            if (p->left != &et_sentinel)
            {
                stack[used] = p->left;
                used++;
            }

            if(p->data->incoming_rel_count[i] == maxes[i])
            {
                fputs(" ",stdout);
                fputs(p->data->name, stdout);
            }
        }
        printf(" %d;", maxes[i]);
        print = 1;
    }

    if(print)
        fputs("\n",stdout);
    else
        fputs("none\n",stdout);

}



static void push_rel_cache(EntityData *ent, RelationData *rel, uu_short is_source)
{
    if (ent->relations_size + 1 > ent->relations_allocated)
    {
        ent->relations_allocated += RELATION_INCREMENT;
        ent->relations = realloc(ent->relations, ent->relations_allocated * sizeof(RelationData *));
    }

    ent->relations[ent->relations_size] = rel;

    if(is_source)
    {
        rel->source_idx = ent->relations_size;
        ent->relations_size++;
    }
    else
    {
        if(rel->from != rel->to)
        {
            rel->dest_idx = ent->relations_size;
            ent->relations_size++;
        }

        ent->incoming_rel_count[rel->relID]++;
    }


}

static void delete_rel_cache(RelationData *rel)
{
    rel->from->relations[rel->source_idx] = NULL;
    if(rel->from != rel->to)
        rel->to->relations[rel->dest_idx] = NULL;

    rel->to->incoming_rel_count[rel->relID]--;

}



int main(int argc, char** argv)
{

    EntityTree *entities = et_init();

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

                        uu_short rel = rel_id_search(command[2]);
                        if (rel > SUPPORTED_RELATION_TYPES)
                        {
                            rel = rel_id_insert(command[2]);
                            res = 1;
                        }

                        int res2;
                        RelationStorageNode *r = rst_insert(&relation_trees[rel], source->data, dest->data, rel, &res2);

                        if (res2)
                        {

                            //add relation to both source and dest

                            push_rel_cache(source->data, r->data, 1);

                            push_rel_cache(dest->data, r->data, 0);
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
                GRAB_CMD_0

                //delete ent

                EntityNode *ent = et_search(entities, command[0]);
                if(ent)
                {
                    EntityData *data = ent->data;
                    RelationData *rdt;
                    for(int i =0; i < data->relations_size; i++)
                    {
                        rdt = data->relations[i];

                        if(rdt != NULL)
                        {
                            delete_rel_cache(rdt);

                            rst_delete(&relation_trees[rdt->relID], rdt->node_ptr);
                        }
                    }

                    et_delete(entities, ent);
                }


                free(command[0]);


            }
            else if(buffer[3] == 'r')
            {
                //delrel <from> <to> <rel>
                GRAB_CMD_0_1_2

                //delrel
                uu_short rel = rel_id_search(command[2]);
                if (rel < SUPPORTED_RELATION_TYPES)  // check valid relation
                {
                    RelationStorageNode *rdt = rst_search(&relation_trees[rel], command[0], command[1]);
                    if(rdt) // existent relation
                    {
                        //decrement/reset caches
                        //TODO: evaluate if arrays suffer from multiple empty spots
                        // can be replaced by duble linked lists

                        delete_rel_cache(rdt->data);

                        rst_delete(&relation_trees[rel], rdt);
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
            report(entities);
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


    for(int i=0; i < last_rel_type_id; i++)
    {
        free(relation_types[i]);
        rst_clean(&relation_trees[i]);
    }


    #ifdef DEBUG
    if(fl) fclose(fl);

    double msTm = (ns() - start_tm)/1000000;
    printf("\nExecution time: %.2fms ~ %.2fs (%.2fns)\n", msTm, msTm/1000, ns() - start_tm);
    #endif

    return 0;
}