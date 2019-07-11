#ifndef __RELATION_HOLDER_TREE__
#define __RELATION_HOLDER_TREE__


#define ET_RED 1
#define ET_BLACK 0

/**
 * Entity tree node
 */
typedef struct s_relationStorageNode
{
    char *name;
    int insert_order;
    int color; // 0 black, 1 red

    struct s_relationStorageNode *parent, *right, *left;
    
} RelationStorageNode;

/**
 * Entity tree holder structure
 */
typedef struct 
{
    int insert_count;
    RelationStorageNode *root;
} RelationStorageTree;


/**
 * Initialize a new entity tree
 *
 * @return
 */
RelationStorageTree *rst_init();


/**
 * Insert a new entity in the tree (duplicate check)
 *
 * @param tree tree where to insert
 * @param entity_name new entity to insert
 * @param 1 if inserted 0 if not
 * @return inserted node pointer
 */
RelationStorageNode *rst_insert(RelationStorageTree *tree, char *entity_name, int *inserted);


/**
 * Search if a key is present in the tree
 *
 * @param tree tree to search
 * @param entity_name entity to search
 * @return pointer to entity node if found, NULL if not found
 */
RelationStorageNode *rst_search(RelationStorageTree *tree, char *entity_name);


/**
 * Search and delete an entity (entity data deleted in this function)
 *
 * @param tree tree where delete
 * @param entity_name entity to delete
 */
void rst_delete(RelationStorageTree *tree, char *entity_name);



/**
 * Deallocate every tree element and it's data
 * tree pointer is not freed
 *
 * @param tree tree to clean
 */
void rst_clean(RelationStorageTree *tree);


//DEBUG

void rst_print(RelationStorageNode *root);

void rst_count(RelationStorageTree *tree);

#endif

