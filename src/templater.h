
#include "rbtree_template.h"

#define COMMA ,



static FORCE_INLINE char *et_allocate(char *data)  { return data; }
static FORCE_INLINE int et_compare(char* x, char *y) { return  strcmp(y, x); }
static FORCE_INLINE void et_deallocate(char *data) {  free(data); }

MAKE_TREE(et, Entity, char *, char *entity , entity, et_allocate, et_compare, et_deallocate, 0)


/*
relations

static FORCE_INLINE char *rel_allocate(char *data)  { return data; }
static FORCE_INLINE int rel_compare(char* x, char *y) { return  strcmp(y, x); }
static FORCE_INLINE void rel_deallocate(char *data) {  free(data); }

MAKE_TREE(rel, RelationName, char *, char *relation , relation, rel_allocate, rel_compare, rel_deallocate, 0)

*/






/*
 * RelationStorage Tree
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

*/
