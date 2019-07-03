#include <stdio.h>

typedef struct s_relationArray
{
    char **relations;
    int size;
    int allocated_size;
} relationArray;

#define RA_DEFAULT_SIZE 10


/**
 * Initialize a relation array
 * 
 * @param arr relation array to initialize
 */
static inline void ra_init(relationArray *arr)
{
    #ifdef DEBUG
    if(!arr)
    {
        DEBUG_PRINT("rb_init: Can't init a null array");
    }
    #endif
    arr->relations = (char **)malloc(sizeof(char**) * RA_DEFAULT_SIZE);
    arr->allocated_size = RA_DEFAULT_SIZE;
    arr->size = 0;
}


/**
 * Insert a new element into the relation array (no duplicate check)
 * 
 * @param arr array where to insert
 * @param rel relation to intert
 */
static inline void ra_insert(relationArray *arr, char *rel)
{

}

/**
 * Check if a key relation is in the array
 * 
 * @param arr array where to search
 * @param rel relation to search
 * @return pointer to reletion char, null if not found
 */
static inline char *ra_find(relationArray *arr, char *rel)
{

}

/**
 * Remove a relation from the array
 * 
 * @param arr array where to remove
 * @param rel relation to remove
 */
static inline void ra_remove(relationArray *arr, char *rel)
{

}

/**
 * Clear all array data and free used memory
 * 
 * @param arr array to delete
 */
static inline void ra_clear(relationArray *arr)
{

}
