#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RH_HASHTABLE_SIZE_DEFAULT 37
#define RH_HASH_PRIME 31

#define RH_MAX_WORSTCASELOOKUP 10
#define RH_MAX_WORSTCASELOOKUPCOUNT 20



/****************************************
 * DATA STRUCTS
 ****************************************/

//hash table item
typedef struct s_rhItem
{
    htItem *to;
    relation *rel;
    int cout;

    struct s_rhItem *next;
} rhItem;

typedef rhItem* rhItemArray;

//hash table structure
typedef struct s_reportTable
{
    rhItemArray *internal; // array of rtItem pointers
    int size; // size of internal

    //rebalancing factors
    int worstLookup; //longest lookup done on the table
    int worstLookupCount; // number of times that worstlookup is done
} reportTable;

// #define VERBODE_DEBUG


/****************************************
 * INIT
 ****************************************/

/**
 * Initialize/reinitialize an hash table
 * 
 * @param size table size
 * @return pointer to allocated table, NULL on error
 */
static inline void rh_init1(reportTable *table, int size)
{
    #ifdef DEBUG
    if(!table)
    {
        DEBUG_PRINT("rh_init: Unable to allocate new table\n");
        return;
    }
    #endif

    table->internal = (rhItemArray *)malloc(sizeof(rhItemArray) * size);
    #ifdef DEBUG
    if(!table->internal)
    {
        DEBUG_PRINT("rh_init: Unable to allocate internal table data\n");
        return;
    }
    #endif
    memset(table->internal, 0, sizeof(rhItemArray) * size);
    table->size = size;
    table->worstLookup = 0;
    table->worstLookupCount = 0;
}

/**
 * Create a new hasrtable
 * 
 * @return pointer to allocated table, NULL on error
 */
static inline reportTable* rh_init2()
{
    reportTable *table = (reportTable *)malloc(sizeof(reportTable));
    #ifdef DEBUG
    if(!table)
    {
        DEBUG_PRINT("rh_init: Unable to allocate new table\n");
        return NULL;
    }
    #endif

    rh_init1(table, RH_HASHTABLE_SIZE_DEFAULT);

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
static inline int rh_hash(htItem* to, relation *rel, int size)
{
    long x = (long)to *(long)rel;
    x = ((x >> 32) ^ x) * 0x45d9f3b;
    x = ((x >> 32) ^ x) * 0x45d9f3b;
    x = (x >> 32) ^ x;
    return x % size;
}


/**
 * Insert key in table (no duplicate check)
 * 
 * @param table hasrtable where insert is made
 * @param item item to insert
 */
static inline void rh_insert1(reportTable *table, rhItem *item)
{
    #ifdef DEBUG
    if(!item)
    {
        DEBUG_PRINT("rh_insert: Can't insert NULL item\n");
        return;
    }
    if(!table)
    {
        DEBUG_PRINT("rh_insert: Unable to allocate internal table data\n");
        return;
    }
    #endif

    int hash = rh_hash(item->to, item->rel, table->size);

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
void rh_insert2(reportTable *table, htItem* to, relation *rel)
{
    rhItem *item = (rhItem *)malloc(sizeof(rhItem));

    #ifdef DEBUG
    if(!item)
    {
        DEBUG_PRINT("ht_insert: Can't allocate new rtItem\n");
        return;
    }
    #endif

    item->cout = 1;
    item->to = to;
    item->rel = rel;

    rh_insert1(table, item);

}


/****************************************
 * RESIZE
 ****************************************/

static inline void rh_resize(reportTable *table)
{
    #ifdef DEBUG
    DEBUG_PRINT("Before resize:\n");
    //rh_print_status(table);
    #endif
    
    int oldsize = table->size;
    rhItemArray *old_data = table->internal;

    rh_init1(table, table->size *2);

    #ifdef DEBUG
    if(!table->internal)
    {
        DEBUG_PRINT("rh_resize: Unable to allocate new table internal data\n");
        return;
    }
    #endif

    for(int i = 0; i < oldsize; i++)
    {
        rhItem  *temp = NULL, *itr = old_data[i];

        while(itr != NULL)
        {
            temp = itr;
            itr = itr->next;
            rh_insert1(table, temp);
        }
    }

    free(old_data);

    #ifdef DEBUG
    DEBUG_PRINT("After resize:\n");
    //rh_print_status(table);
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
rhItem* rh_hasKey(reportTable *table, htItem* to, relation *rel)
{
    #ifdef DEBUG
    if(!table)
    {
        DEBUG_PRINT("rh_search: Can't search a null table\n");
        return NULL;
    }
    #endif

    int hash = rh_hash(to, rel, table->size);
    int lookup = 0;

    if(table->internal[hash])
    {
        rhItem *itr = table->internal[hash];
        while(itr)
        {
            if(itr->to == to && itr->rel == rel) return itr;
            itr = itr->next;
            lookup++;
        }
    }

    //autoresize
    if(lookup >= table->worstLookup)
    {
        if(lookup > RH_MAX_WORSTCASELOOKUP)
        {
            DEBUG_PRINT("rh_search: Resizing table due to max lookup hit\n");
            rh_resize(table);
            DEBUG_PRINT("\n");
        }
        else
        {
            table->worstLookupCount = lookup;
            table->worstLookupCount++;
            if(table->worstLookupCount > RH_MAX_WORSTCASELOOKUPCOUNT)
            {
                DEBUG_PRINT("rh_search: Resizing table due to max lookup count hit\n");
                rh_resize(table);
                DEBUG_PRINT("\n");
            }
        }

    }

    return 0;
}


/****************************************
 * DELETION
 ****************************************/

// this table is desegned to only do insert operation because it shoud be allocated every operation

/****************************************
 * CLEAN UP
 ****************************************/

/**
 * Delete all allocated ram for table
 * This function also deletes ALL key strings!
 */
void rh_clean(reportTable *table)
{
    #ifdef DEBUG
    if(!table)
    {
        DEBUG_PRINT("rh_clean: Can't delete a null table\n");
        return;
    }
    #endif
    for(int i = 0; i < table->size; i++)
    {
        rhItem *del = NULL, *itr = table->internal[i];
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
