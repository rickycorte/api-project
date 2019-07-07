#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RT_HASHTABLE_SIZE_DEFAULT 37
#define RT_HASH_PRIME 31

#define RT_MAX_WORSTCASELOOKUP 10
#define RT_MAX_WORSTCASELOOKUPCOUNT 20



/****************************************
 * DATA STRUCTS
 ****************************************/

//hash table item
typedef struct s_rtItem
{
    htItem *from;
    htItem *to;
    relation *rel;

    struct s_rtItem *next;
} rtItem;

typedef rtItem* rtItemArray;

//hash table structure
typedef struct s_relationtable
{
    rtItemArray *internal; // array of rtItem pointers
    int size; // size of internal

    //rebalancing factors
    int worstLookup; //longest lookup done on the table
    int worstLookupCount; // number of times that worstlookup is done
} relationTable;

// #define VERBODE_DEBUG

/****************************************
 * DEBUG
 ****************************************/

#ifdef DEBUG

/**
 * Print table status
 * 
 * @param table table to print
 */
void rt_print_status(relationTable *table)
{
    if(!table) return;

    int display = 0;
    for(int i = 0; i < table->size; i++)
    {
        if(table->internal[i] != NULL) display++;
    }

    int elem_count = 0,templd = 0, worst_load = 0;

    DEBUG_PRINT("Bucket status: %d used of %i:\n", display, table->size);

    #ifdef VERBOSE_DEBUG
    DEBUG_PRINT("   hash   |  bucket\n");
    #endif

    for(int i = 0; i < table->size; i++)
    {
        rtItem *itr = table->internal[i];

        #ifdef VERBOSE_DEBUG
        DEBUG_PRINT("%9d | ",i);
        #endif
        templd = 0;
        while (itr)
        {
            itr = itr->next;
            #ifdef VERBOSE_DEBUG
            DEBUG_PRINT("x");
            #endif
            elem_count++;
            templd++;

        } 
        if(templd > worst_load) worst_load = templd;

        #ifdef VERBOSE_DEBUG
        DEBUG_PRINT("\n");
        #endif
    }

    DEBUG_PRINT("RT: Element count: %d, avg load: %.2f, worst load: %d\n", elem_count, ((double)elem_count) / display, worst_load);
}


#endif



/****************************************
 * INIT
 ****************************************/

/**
 * Initialize/reinitialize an hash table
 * 
 * @param size table size
 * @return pointer to allocated table, NULL on error
 */
static inline void rt_init1(relationTable *table, int size)
{
    #ifdef DEBUG
    if(!table)
    {
        DEBUG_PRINT("rt_init: Unable to allocate new table\n");
        return;
    }
    #endif

    table->internal = (rtItemArray *)malloc(sizeof(rtItemArray) * size);
    #ifdef DEBUG
    if(!table->internal)
    {
        DEBUG_PRINT("rt_init: Unable to allocate internal table data\n");
        return;
    }
    #endif
    memset(table->internal, 0, sizeof(rtItemArray) * size);
    table->size = size;
    table->worstLookup = 0;
    table->worstLookupCount = 0;
}

/**
 * Create a new hasrtable
 * 
 * @return pointer to allocated table, NULL on error
 */
static inline relationTable* rt_init2()
{
    relationTable *table = (relationTable *)malloc(sizeof(relationTable));
    #ifdef DEBUG
    if(!table)
    {
        DEBUG_PRINT("rt_init: Unable to allocate new table\n");
        return NULL;
    }
    #endif

    rt_init1(table, RT_HASHTABLE_SIZE_DEFAULT);

    return table;
}


/****************************************
 * INSERTION
 ****************************************/

/**
 * Compute hash based on table size
 * @param key string to hash
 * @param size table to size
 * 
 * @return key hash
 */
static inline int rt_hash(htItem *from, htItem* to, relation *rel, int size)
{
    long x = (long)from * (long)to *(long)rel;
    x = ((x >> 32) ^ x) * 0x45d9f3b;
    x = ((x >> 32) ^ x) * 0x45d9f3b;
    x = (x >> 32) ^ x;
    return x %size;
}


/**
 * Insert key in table (no duplicate check)
 * 
 * @param table hasrtable where insert is made
 * @param item item to insert
 */
static inline void rt_insert1(relationTable *table, rtItem *item)
{
    #ifdef DEBUG
    if(!item)
    {
        DEBUG_PRINT("rt_insert: Can't insert NULL item\n");
        return;
    }
    if(!table)
    {
        DEBUG_PRINT("rt_insert: Unable to allocate internal table data\n");
        return;
    }
    #endif

    int hash = rt_hash(item->from, item->to, item->rel, table->size);

    if(table->internal[hash])
    {
        item->next = table->internal[hash];
    }
    else
    {
        item->next = NULL;
    } 
    
    table->internal[hash] = item;
}


/**
 * Insert key in table (no duplicate check)
 * 
 * @param table hasrtable where insert is made
 * @param key key to insert
 */
void rt_insert2(relationTable *table, htItem* from, htItem* to, relation *rel)
{
    rtItem *item = (rtItem *)malloc(sizeof(rtItem));

    #ifdef DEBUG
    if(!item)
    {
        DEBUG_PRINT("rt_insert: Can't allocate new rtItem\n");
        return;
    }
    #endif

    item->from = from;
    item->to = to;
    item->rel = rel;

    rt_insert1(table, item);

    #ifdef OPERATIONS
    DEBUG_PRINT(" ! Created relation from: %s (%p) to: %s (%p) rel: %s (%p)\n",
     item->from->data, item->from, item->to->data, item->to, item->rel->name, item->rel);
     #endif
}


/****************************************
 * RESIZE
 ****************************************/

static inline void rt_resize(relationTable *table)
{
    #ifdef DEBUG
    DEBUG_PRINT("Before resize:\n");
    rt_print_status(table);
    #endif
    
    int oldsize = table->size;
    rtItemArray *old_data = table->internal;

    rt_init1(table, table->size *2);

    #ifdef DEBUG
    if(!table->internal)
    {
        DEBUG_PRINT("rt_resize: Unable to allocate new table internal data\n");
        return;
    }
    #endif

    for(int i = 0; i < oldsize; i++)
    {
        rtItem  *temp = NULL, *itr = old_data[i];

        while(itr != NULL)
        {
            temp = itr;
            itr = itr->next;
            rt_insert1(table, temp);
        }
    }

    free(old_data);

    #ifdef DEBUG
    DEBUG_PRINT("After resize:\n");
    rt_print_status(table);
    #endif
}


/****************************************
 * LOOKUP
 ****************************************/

/**
 * Search for a key in an hash table
 * If lookups are too slow, this function resizes the hash table!
 * 
 * @param table pointer to table where to find key
 * @param key key to search
 * @return rtItem pointer of found element, null if not found
 */
rtItem* rt_hasKey(relationTable *table, htItem* from, htItem* to, relation *rel)
{
    #ifdef DEBUG
    if(!table)
    {
        DEBUG_PRINT("rt_search: Can't search a null table\n");
        return NULL;
    }
    #endif

    int hash = rt_hash(from, to, rel, table->size);
    int lookup = 0;

    if(table->internal[hash])
    {
        rtItem *itr = table->internal[hash];
        while(itr)
        {
            if(itr->from == from && itr->to == to && itr->rel == rel) return itr;
            itr = itr->next;
            lookup++;
        }
    }

    //autoresize
    if(lookup >= table->worstLookup)
    {
        if(lookup > RT_MAX_WORSTCASELOOKUP)
        {
            DEBUG_PRINT("rt_search: Resizing table due to max lookup hit\n");
            rt_resize(table);
            DEBUG_PRINT("\n");
        }
        else
        {
            table->worstLookupCount = lookup;
            table->worstLookupCount++;
            if(table->worstLookupCount > RT_MAX_WORSTCASELOOKUPCOUNT)
            {
                DEBUG_PRINT("rt_search: Resizing table due to max lookup count hit\n");
                rt_resize(table);
                DEBUG_PRINT("\n");
            }
        }

    }

    return 0;
}


/****************************************
 * DELETION
 ****************************************/

/**
 * Delete key from table
 * 
 * @param table pointer to table where to find key
 * @param key key to search and delete
 */
void rt_remove(relationTable *table, htItem* from, htItem* to, relation *rel)
{
    #ifdef DEBUG
    if(!table)
    {
        DEBUG_PRINT("rt_remove: Can't search a null table\n");
        return;
    }
    #endif

    int hash = rt_hash(from, to, rel, table->size);
    rtItem *del = NULL, *itr = table->internal[hash];

    if(itr)
    {       
        if(itr->from == from && itr->to == to && itr->rel == rel)
        {
            del = itr;
            table->internal[hash] = itr->next;
        }
        else
        {
            while(itr->next)
            {             
                if(itr->next->from == from && itr->next->to == to && itr->next->rel == rel)
                {
                    //found match
                    del = itr->next;
                    itr->next = del->next;
                    break;
                }
                itr = itr->next;
            }
        }
        if(del)
        {
            #ifdef OPERATIONS
            DEBUG_PRINT(" $ Deleted relation from: %s (%p) to: %s (%p) rel: %s (%p)\n",
                del->from->data, del->from, del->to->data, del->to, del->rel->name, del->rel);
                #endif
            free(del);
        }
    }
    #ifdef DEBUG
    else
    {
        //not found
        //DEBUG_PRINT("rt_remove: Trying to delete not existent key for %s\n", key);
    }
    #endif

}


/**
 * Remove every relation of an entity
 * 
 * @param table table where to remove
 * @param ent target of future removal
 */
void rt_removeAll_for(relationTable *table, htItem* ent)
{

    for(int i = 0; i < table->size; i++)
    {
        rtItem *prev = NULL, *del = NULL, *itr = table->internal[i];
       
        while (itr)
        {
            if(itr->from == ent || itr->to == ent)
            {
                del = itr;
                if(del == table->internal[i])
                {
                    table->internal[i] = itr->next;
                }
                else
                {
                    prev->next = del->next;
                }              
            }
            else
            {
                prev = itr;
            }
            itr = itr->next;

            if(del) 
            {
                #ifdef OPERATIONS
                DEBUG_PRINT(" $ Deleted relation from: %s (%p) to: %s (%p) rel: %s (%p)\n",
                del->from->data, del->from, del->to->data, del->to, del->rel->name, del->rel);
                #endif
                free(del);
                del = NULL;
            }

        } 
    }
}

/****************************************
 * CLEAN UP
 ****************************************/

/**
 * Delete all allocated ram for table
 * This function also deletes ALL key strings!
 */
void rt_clean(relationTable *table)
{
    #ifdef DEBUG
    if(!table)
    {
        DEBUG_PRINT("rt_clean: Can't delete a null table\n");
        return;
    }
    #endif
    for(int i = 0; i < table->size; i++)
    {
        rtItem *del = NULL, *itr = table->internal[i];
        while (itr)
        {
            del = itr;
            itr = itr->next;
  
            free(del);
        }     
    }

    free(table->internal);
    free(table);
}
