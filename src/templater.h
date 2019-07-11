
#include "rbtree_template.h"

#define COMMA ,

/*
 * RelationStorage Tree
*/
typedef struct
{
    char *from;
    char *to;
    char *rel;
}RelationStorageData;

static inline RelationStorageData *rst_allocate(char *from, char* to, char* rel)
{
    RelationStorageData *dt = malloc(sizeof(RelationStorageData));
    dt->from = from;
    dt->to = to;
    dt->rel = rel;

    return dt;
}

static inline int rst_compare(RelationStorageData * x, char *from, char* to, char* rel)
{
    if(from == x->from && to == x->to && rel == x->rel)
        return 0;

    int res = from > x->from && to > x->to && rel > x->rel;

    return res != 0 ? 1 : -1;
}

static inline void rst_deallocate(RelationStorageData *data)
{
    //dont free strings, they are freed by other trees
    free(data);
}

MAKE_TREE(rst, RelationStorage, RelationStorageData *,
char *from COMMA char* to COMMA char* rel , from COMMA to COMMA rel,
        rst_allocate, rst_compare, rst_deallocate, {0 COMMA 0 COMMA 0} )