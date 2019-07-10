#ifndef __ENITY_TREE__
#define __ENTITY_TREE__


#define ET_RED 1
#define ET_BLACK 0

/**
 * Entity tree node
 */
typedef struct s_entityNode
{
    char *name;
    int color; // 0 black, 1 red

    struct s_entityNode *parent, *right, *left;
    
} EntityNode;

/**
 * Entity tree holder structure
 */
typedef struct 
{
    EntityNode *root;
} EntityTree;


/**
 * Initialize a new entity tree
 *
 * @return
 */
EntityTree *et_init();


/**
 * Insert a new entity in the tree (duplicate check)
 *
 * @param tree tree where to insert
 * @param entity_name new entity to insert
 * @return 1 if inserted a new node, 0 if duplicate
 */
int et_insert(EntityTree *tree, char *entity_name);


/**
 * Search if a key is present in the tree
 *
 * @param tree tree to search
 * @param entity_name entity to search
 * @return pointer to entity node if found, NULL if not found
 */
EntityNode *et_search(EntityTree *tree, char *entity_name);


/**
 * Search and delete an entity (entity data deleted in this function)
 *
 * @param tree tree where delete
 * @param entity_name entity to delete
 */
void et_delete(EntityTree *tree, char *entity_name);



/**
 * Deallocate every tree element and it's data
 * tree pointer is not freed
 *
 * @param tree tree to clean
 */
void et_clean(EntityTree *tree);


//DEBUG

void et_print(EntityNode *root);

void et_count(EntityTree *tree);

#endif

