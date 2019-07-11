#ifndef CETRIOLI_RELATION_STORAGE_TREE_H
#define CETRIOLI_RELATION_STORAGE_TREE_H

#define ET_RED 1
#define ET_BLACK 0

/**
 * Entity tree node
 */
typedef struct s_relationNode
{
    char *from;
    char *to;
    char *rel;

    int color; // 0 black, 1 red

    struct s_relationNode *parent, *right, *left;

} RelationNameNode;

/**
 * Entity tree holder structure
 */
typedef struct
{
    int insert_count;
    RelationNameNode *root;
} RelationNameTree;


/**
 * Initialize a new entity tree
 *
 * @return
 */
RelationNameTree *rht_init();


/**
 * Insert a new entity in the tree (duplicate check)
 *
 * @param tree tree where to insert
 * @param entity_name new entity to insert
 * @param 1 if inserted 0 if not
 * @return inserted node pointer
 */
RelationNameNode *rht_insert(RelationNameTree *tree, char *entity_name, int *inserted);


/**
 * Search if a key is present in the tree
 *
 * @param tree tree to search
 * @param entity_name entity to search
 * @return pointer to entity node if found, NULL if not found
 */
RelationNameNode *rht_search(RelationNameTree *tree, char *entity_name);


/**
 * Search and delete an entity (entity data deleted in this function)
 *
 * @param tree tree where delete
 * @param entity_name entity to delete
 */
void rht_delete(RelationNameTree *tree, char *entity_name);



/**
 * Deallocate every tree element and it's data
 * tree pointer is not freed
 *
 * @param tree tree to clean
 */
void rht_clean(RelationNameTree *tree);


//DEBUG

void rht_count(RelationNameTree *tree);

#endif //CETRIOLI_RELATION_STORAGE_TREE_H
