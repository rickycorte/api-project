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
    struct ht_item *next;
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


/****************************************
 * INIT
 ****************************************/

/**
 * Create a new hashtable
 * 
 * @return pointer to allocated table, NULL on error
 */
static inline hashTable* ht_init()
{
    hashTable *table = (hashTable *)malloc(sizeof(hashTable));
    #ifdef DEBUG
    if(!table)
    {
        DEBUG_PRINT("ht_init: Unable to allocate new table");
        return NULL;
    }
    #endif

    table->internal = (htItemArray *)malloc(sizeof(htItemArray) * HT_HASHTABLE_SIZE_DEFAULT);
    #ifdef DEBUG
    if(!table->internal)
    {
        DEBUG_PRINT("ht_init: Unable to allocate internal table data");
        return NULL;
    }
    #endif
    memset(table->internal, 0, sizeof(htItem) * HT_HASHTABLE_SIZE_DEFAULT);
    table->size = HT_HASHTABLE_SIZE_DEFAULT;
    table->worstLookup = 0;
    table->worstLookupCount = 0;

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
 * @param key key to insert
 */
void ht_insert(hashTable *table, char *key)
{
    #ifdef DEBUG
    if(!key)
    {
        DEBUG_PRINT("ht_insert: Can't hash a NULL key");
        return NULL;
    }
    if(!table)
    {
        DEBUG_PRINT("ht_insert: Unable to allocate internal table data");
        return NULL;
    }
    #endif

    htItem *item = (htItem *)malloc(sizeof(htItem));

    #ifdef DEBUG
    if(!item)
    {
        DEBUG_PRINT("ht_insert: Can't allocate new htItem");
        return NULL;
    }
    #endif

    int hash = ht_hash(key, table->size);
    item->data = key;
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
        DEBUG_PRINT("ht_search: Can't search a NULL key");
        return 0;
    }
    if(!table)
    {
        DEBUG_PRINT("ht_search: Can't search a null table");
        return 0;
    }
    #endif

    int hash = ht_hash(key, table->size);

    if(table->internal[hash])
    {
        htItem *itr = table->internal[hash];
        while(itr)
        {
            if(strcmp(key, itr->data) == 0) return 1;
            itr = itr->next;
        }
    }

    return 0;
}


/****************************************
 * RESIZE
 ****************************************/


/****************************************
 * DELETION
 ****************************************/


/****************************************
 * DEBUG
 ****************************************/