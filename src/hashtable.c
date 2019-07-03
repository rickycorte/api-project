#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HT_HASHTABLE_SIZE_DEFAULT 37
#define HT_HASH_PRIME 31

#define HT_MAX_WORSTCASELOOKUP 10
#define HT_MAX_WORSTCASELOOKUPCOUNT 5



/****************************************
 * DATA STRUCTS
 ****************************************/

//hash table item
typedef struct s_htItem
{
    char *data;
    struct s_htItem *next;
} htItem;

typedef htItem* htItemArray;

//hash table structure
typedef struct s_hashtable
{
    htItemArray *internal; // array of htItem pointers
    int size; // size of internal

    //rebalancing factors
    int worstLookup; //longest lookup done on the table
    int worstLookupCount; // number of times that worstlookup is done
} hashTable;

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
void ht_print_status(hashTable *table)
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
        htItem *itr = table->internal[i];

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

    DEBUG_PRINT("Element count: %d, avg load: %.2f, worst load: %d\n", elem_count, ((double)elem_count) /table->size, worst_load);
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
static inline void ht_init1(hashTable *table, int size)
{
    #ifdef DEBUG
    if(!table)
    {
        DEBUG_PRINT("ht_init: Unable to allocate new table\n");
        return;
    }
    #endif

    table->internal = (htItemArray *)malloc(sizeof(htItemArray) * size);
    #ifdef DEBUG
    if(!table->internal)
    {
        DEBUG_PRINT("ht_init: Unable to allocate internal table data\n");
        return;
    }
    #endif
    memset(table->internal, 0, sizeof(htItemArray) * size);
    table->size = size;
    table->worstLookup = 0;
    table->worstLookupCount = 0;
}

/**
 * Create a new hashtable
 * 
 * @return pointer to allocated table, NULL on error
 */
static inline hashTable* ht_init2()
{
    hashTable *table = (hashTable *)malloc(sizeof(hashTable));
    #ifdef DEBUG
    if(!table)
    {
        DEBUG_PRINT("ht_init: Unable to allocate new table\n");
        return NULL;
    }
    #endif

    ht_init1(table, HT_HASHTABLE_SIZE_DEFAULT);

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
static inline int ht_hash(char *key, int size)
{
    #ifdef DEBUG
    if(!key)
    {
        DEBUG_PRINT("ht_hash: Can't hash a NULL key");
        return NULL;
    }
    #endif

    unsigned long res = *key;
    key++;

    unsigned long pow = HT_HASH_PRIME;

    for(int i = 1; *key != '\0'; key++, i++)
    {
        res += (*key) * pow;
        pow *= HT_HASH_PRIME;
    }
    return res % size;
}


/**
 * Insert key in table (no duplicate check)
 * 
 * @param table hashtable where insert is made
 * @param item item to insert
 */
static inline void ht_insert1(hashTable *table, htItem *item)
{
    #ifdef DEBUG
    if(!item)
    {
        DEBUG_PRINT("ht_insert: Can't insert NULL item\n");
        return NULL;
    }
    if(!table)
    {
        DEBUG_PRINT("ht_insert: Unable to allocate internal table data\n");
        return NULL;
    }
    #endif

    int hash = ht_hash(item->data, table->size);

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
 * @param table hashtable where insert is made
 * @param key key to insert
 */
void ht_insert2(hashTable *table, char *key)
{
    #ifdef DEBUG
    if(!key)
    {
        DEBUG_PRINT("ht_insert: Can't hash a NULL key\n");
        return NULL;
    }
    #endif

    htItem *item = (htItem *)malloc(sizeof(htItem));

    #ifdef DEBUG
    if(!item)
    {
        DEBUG_PRINT("ht_insert: Can't allocate new htItem\n");
        return NULL;
    }
    #endif

    item->data = key;

    ht_insert1(table, item);
}


/****************************************
 * RESIZE
 ****************************************/

static inline void ht_resize(hashTable *table)
{
    #ifdef DEBUG
    DEBUG_PRINT("Before resize:\n");
    ht_print_status(table);
    #endif
    
    int oldsize = table->size;
    htItemArray *old_data = table->internal;

    ht_init1(table, table->size *2);

    #ifdef DEBUG
    if(!table->internal)
    {
        DEBUG_PRINT("ht_resize: Unable to allocate new table internal data\n");
        return;
    }
    #endif

    for(int i = 0; i < oldsize; i++)
    {
        htItem  *temp = NULL, *itr = old_data[i];

        while(itr != NULL)
        {
            temp = itr;
            itr = itr->next;
            ht_insert1(table, temp);
        }
    }

    free(old_data);

    #ifdef DEBUG
    DEBUG_PRINT("After resize:\n");
    ht_print_status(table);
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
 * @return true if found, false otherwise
 */
int ht_hasKey(hashTable *table, char* key)
{
    #ifdef DEBUG
    if(!key)
    {
        DEBUG_PRINT("ht_search: Can't search a NULL key\n");
        return 0;
    }
    if(!table)
    {
        DEBUG_PRINT("ht_search: Can't search a null table\n");
        return 0;
    }
    #endif

    int hash = ht_hash(key, table->size);
    int lookup = 0;

    if(table->internal[hash])
    {
        htItem *itr = table->internal[hash];
        while(itr)
        {
            #ifdef DEBUG
            if(!itr->data)
            {
                DEBUG_PRINT("Ohu now we lost data at %p", itr);
            }
            #endif

            if(strcmp(key, itr->data) == 0) return 1;
            itr = itr->next;
            lookup++;
        }
    }

    //autoresize
    if(lookup >= table->worstLookup)
    {
        if(lookup > HT_MAX_WORSTCASELOOKUP)
        {
            ht_resize(table);
            DEBUG_PRINT("ht_search: Resizing table due to max lookup hit\n");
        }
        else
        {
            table->worstLookupCount = lookup;
            table->worstLookupCount++;
            if(table->worstLookupCount > HT_MAX_WORSTCASELOOKUPCOUNT)
            {
                ht_resize(table);
                DEBUG_PRINT("ht_search: Resizing table due to max lookup count hit\n");
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
void ht_remove(hashTable *table, char* key)
{
    #ifdef DEBUG
    if(!key)
    {
        DEBUG_PRINT("ht_remove: Can't search a NULL key\n");
        return;
    }
    if(!table)
    {
        DEBUG_PRINT("ht_remove: Can't search a null table\n");
        return;
    }
    #endif

    int hash = ht_hash(key, table->size);
    htItem *del = NULL, *itr = table->internal[hash];

    if(itr)
    {       
        if(strcmp(itr->data, key) == 0)
        {
            del = itr;
            table->internal[hash] = itr->next;
        }
        else
        {
            while(itr->next)
            {             
                if(strcmp(key, itr->data) == 0)
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
            free(del->data);
            free(del);
        }
    }
    #ifdef DEBUG
    else
    {
        //not found
        //DEBUG_PRINT("ht_remove: Trying to delete not existent key for %s\n", key);
    }
    #endif

}


/****************************************
 * CLEAN UP
 ****************************************/

/**
 * Delete all allocated ram for table
 * This function also deletes ALL key strings!
 */
void ht_clean(hashTable *table)
{
    #ifdef DEBUG
    if(!table)
    {
        DEBUG_PRINT("ht_clean: Can't delete a null table\n");
        return;
    }
    #endif
    for(int i = 0; i < table->size; i++)
    {
        htItem *del = NULL, *itr = table->internal[i];
        while (itr)
        {
            del = itr;
            itr = itr->next;

            free(del->data);       
            free(del);
        }     
    }

    free(table->internal);
    free(table);
}
