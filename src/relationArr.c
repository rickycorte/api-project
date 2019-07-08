#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define RA_DEFAULT_SIZE 100
#define RA_DEFAULT_RESIZE 5


/****************************************
 * DATA STRUCTS
 ****************************************/

typedef struct s_relation
{
    char* name;
    int index;
    int len;
} relation;

typedef struct s_relationArray
{
    relation **relations;
    int size;
    int allocated_size;
} relationArray;



/****************************************
 * INIT
 ****************************************/

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
    arr->relations = (relation **)malloc(sizeof(relation**) * RA_DEFAULT_SIZE);
    arr->allocated_size = RA_DEFAULT_SIZE;
    arr->size = 0;
}



/****************************************
 * INSERT
 ****************************************/

/**
 * Insert a new element into the relation array (no duplicate check)
 * Insertion is ordered!
 * 
 * @param arr array where to insert
 * @param rel relation to intert
 * @param len lenght of rel
 */
static inline relation *ra_insert(relationArray *arr, char *rel, int len)
{
    #ifdef DEBUG
    if(!arr)
    {
        DEBUG_PRINT("rb_insert: Can't insert in a null array");
        return NULL;
    }
    if(!rel)
    {
        DEBUG_PRINT("rb_find1: Can't insert a null relation");
        return NULL;
    }
    #endif

    relation *irel = malloc(sizeof(relation));
    irel->name = rel;
    irel->len = len;

    //resize if no more space is available
    if(arr->size + 1 > arr->allocated_size)
    {
       relation **temp = realloc(arr->relations, (arr->allocated_size + RA_DEFAULT_RESIZE) * sizeof(relation**));
       if(!temp)
       {
           DEBUG_PRINT("ra_insert: Unable to reallocate relation array");
       }
       else
       {
           arr->relations = temp;
           arr->allocated_size += RA_DEFAULT_RESIZE;
       }
    }

    //insert
    int i;
    for(i = arr->size - 1;  i >= 0 && strcmp(rel, arr->relations[i]->name) < 0; i--)
    { 
        arr->relations[i+1] = arr->relations[i];
        arr->relations[i+1]->index = i+1;
    }
    arr->relations[i+1] = irel;
    arr->relations[i+1]->index = i+1;

    arr->size += 1;

    return irel;

}


/****************************************
 * LOOKUP
 ****************************************/

/**
 * Check if a key relation is in the array
 * 
 * @param arr array where to search
 * @param rel relation to search
 * @return index of found element, -1 if not found
 */
static inline int ra_find1(relationArray *arr, char *rel)
{
    #ifdef DEBUG
    if(!arr)
    {
        DEBUG_PRINT("rb_find1: Can't search a null array");
        return -1;
    }
    if(!rel)
    {
        DEBUG_PRINT("rb_find1: Can't search a null relation");
        return -1;
    }
    #endif


    int high = arr->size-1, low = 0, cmp = 0;
    int mid = 0;

    while (low <= high)
    {
        mid = (low + high)/2;
        cmp = strcmp(rel, arr->relations[mid]->name);
        if(cmp == 0)
        {
            return mid;
        }
        else if(cmp > 0)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

    return -1; 
}


/**
 * Check if a key relation is in the array
 * 
 * @param arr array where to search
 * @param rel relation to search
 * @return pointer to reletion char, null if not found
 */
static inline relation *ra_find2(relationArray *arr, char *rel)
{
    int idx = ra_find1(arr, rel);

    return (idx >= 0)? arr->relations[idx] : NULL; 
}



/****************************************
 * REMOVE
 ****************************************/

/**
 * Remove a relation from the array
 * 
 * @param arr array where to remove
 * @param rel relation to remove
 */
static inline void ra_remove(relationArray *arr, char *rel)
{
    int pos = ra_find1(arr, rel);

    if(pos = -1) return;
    
    for(int i = pos; i < arr->size-1; i++)
    {
        arr->relations[i] = arr->relations[i+1]; 
        arr->relations[i]->index = i;
    }
    arr->size -= 1;
}



/****************************************
 * CLEANUP
 ****************************************/

/**
 * Clear all array data and free used memory
 * 
 * @param arr array to delete
 */
static inline void ra_clean(relationArray *arr)
{
    #ifdef DEBUG
    if(!arr)
    {
        DEBUG_PRINT("rb_clear: Can't clear a null array");
        return;
    }
    #endif

    for(int i = 0; i < arr->size; i++)
    {
        free(arr->relations[i]->name);
        free(arr->relations[i]);
    }
    free(arr->relations);
}
