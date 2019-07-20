#include <stdlib.h>

#define SA_DEFAULT_ALLOCATION 10
#define SA_REALLOC_INCREMENT 10

/**************************************************************
 *
 * TYPES
 *
 ***************************************************************/

typedef void *(*sa_allocator)(void *);
typedef int (*sa_comparator)(void *, void *);
typedef void (*sa_deallocator)(void *);

typedef struct
{
    void **arr;
    size_t size;
    size_t allocated;

    sa_comparator comparator;

    //optional
    sa_allocator allocator;
    sa_deallocator deallocator;
} SortedArray;


/**************************************************************
 *
 * INIT
 *
 ***************************************************************/

/**
 * Initialize array
 * @param comparator
 * @return
 */
SortedArray *sa_init(sa_comparator comparator)
{
    SortedArray *sa = malloc(sizeof(SortedArray));

    sa->comparator = comparator;
    sa->arr = malloc(sizeof(void *) * SA_DEFAULT_ALLOCATION);
    sa->size = 0;
    sa->allocated = SA_DEFAULT_ALLOCATION;
    sa->allocator = NULL;
    sa->deallocator = NULL;
}


/**************************************************************
 *
 * INSERT
 *
 ***************************************************************/

/**
 * Insert a new element in the array (no duplicate check)
 *
 * @param sa
 * @param item
 * @return NULL on error, Pointer to element if inserted
 */
void *sa_insert(SortedArray *sa, void *item)
{
    if(!sa || !item)
        return NULL;

    //need more space
    if(sa->size + 1 > sa->allocated)
    {
        sa->allocated += SA_REALLOC_INCREMENT;
        sa->arr = realloc(sa->arr, sizeof(void *) * sa->allocated);
    }

    //insert in order
    int i;
    for(i = sa->size - 1; i >= 0 && sa->comparator(item, sa->arr[i]); i--)
    {
        sa->arr[i+1] = sa->arr[i];
    }

    item = (sa->allocator) ? sa->allocator(item) : item;

    sa->arr[i+1] = item;

    return item;
}

/**************************************************************
 *
 * SEARCH
 *
 ***************************************************************/

/**
 * Binary search item in array
 * @param sa
 * @param item
 * @return index where found, -1 if not found
 */
int sa_seach1(SortedArray *sa, void *item)
{
    int high = sa->size-1, low = 0, cmp = 0;
    int mid = 0;

    while (low <= high)
    {
        mid = (low + high)/2;
        cmp = sa->comparator(item, sa->arr[mid]);

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
 * Binary search item in array
 *
 * @param sa
 * @param item
 * @return item if found, NULL if not found
 */
void *sa_search2(SortedArray *sa, void *item)
{
    int res = sa_seach1(sa, item);

    return (res >= 0) ? sa->arr[res] : NULL;
}

/**************************************************************
 *
 * DELETE
 *
 ***************************************************************/

/**
 * Delete an item from the sorted array
 * @param sa
 * @param item
 * @return 1 on delete, 0 not found
 */
int ra_remove(SortedArray *sa, void *item)
{
    int pos = sa_seach1(sa, item);

    if(pos == -1) return 0;

    if(sa->deallocator) sa->deallocator(sa->arr[pos]);

    for(int i = pos; i < sa->size-1; i++)
    {
        sa->arr[i] = sa->arr[i+1];
    }
    sa->size -= 1;

    return 1;
}

/**************************************************************
 *
 * CLEAN
 *
 ***************************************************************/

/**
 * Delete allocated ram
 * @param sa
 */
void sa_clean(SortedArray *sa)
{
    if(!sa) return;

    if(sa->deallocator)
    {
        for(int i =0; i < sa->size; i++)
            sa->deallocator(sa->arr[i]);
    }

    free(sa->arr);
    free(sa);
}