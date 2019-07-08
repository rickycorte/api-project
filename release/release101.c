#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************************************************************************************************
 *
 *  HASH TABLE
 * 
 **************************************************************************************************/

#define HT_HASHTABLE_SIZE_DEFAULT 2048
#define HT_HASH_PRIME 31

#define HT_MAX_WORSTCASELOOKUP 5
#define HT_MAX_WORSTCASELOOKUPCOUNT 20


/****************************************
 * DATA STRUCTS
 ****************************************/

//hash table item
typedef struct s_htItem
{
    char *data;
    int len;
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

    table->internal = (htItemArray *)malloc(sizeof(htItemArray) * size);
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
 * @param len key lenght
 */
void ht_insert2(hashTable *table, char *key, int len)
{

    htItem *item = (htItem *)malloc(sizeof(htItem));

    item->data = key;
    item->len = len;

    ht_insert1(table, item);
}


/****************************************
 * RESIZE
 ****************************************/

static inline void ht_resize(hashTable *table)
{
    
    int oldsize = table->size;
    htItemArray *old_data = table->internal;

    ht_init1(table, table->size *2);


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
 * @return htItem pointer of found element, null if not found
 */
htItem* ht_hasKey(hashTable *table, char* key)
{

    int hash = ht_hash(key, table->size);
    int lookup = 0;

    if(table->internal[hash])
    {
        htItem *itr = table->internal[hash];
        while(itr)
        {
            if(strcmp(key, itr->data) == 0) return itr;
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
        }
        else
        {
            table->worstLookupCount = lookup;
            table->worstLookupCount++;
            if(table->worstLookupCount > HT_MAX_WORSTCASELOOKUPCOUNT)
            {
                ht_resize(table);
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
            while(itr)
            {             
                if(strcmp(key, itr->next->data) == 0)
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



/**************************************************************************************************
 *
 *  RELATION ARRAY
 * 
 **************************************************************************************************/


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

    relation *irel = malloc(sizeof(relation));
    irel->name = rel;
    irel->len = len;

    //resize if no more space is available
    if(arr->size + 1 > arr->allocated_size)
    {
       relation **temp = realloc(arr->relations, (arr->allocated_size + RA_DEFAULT_RESIZE) * sizeof(relation**));
        if(temp)
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
    for(int i = 0; i < arr->size; i++)
    {
        free(arr->relations[i]->name);
        free(arr->relations[i]);
    }
    free(arr->relations);
}


/**************************************************************************************************
 *
 *  RELATION HASH TABLE
 * 
 **************************************************************************************************/


#define RT_HASHTABLE_SIZE_DEFAULT 2048
#define RT_HASH_PRIME 31

#define RT_MAX_WORSTCASELOOKUP 5
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
    table->internal = (rtItemArray *)malloc(sizeof(rtItemArray) * size);

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

    item->from = from;
    item->to = to;
    item->rel = rel;

    rt_insert1(table, item);

}


/****************************************
 * RESIZE
 ****************************************/

static inline void rt_resize(relationTable *table)
{
    
    int oldsize = table->size;
    rtItemArray *old_data = table->internal;

    rt_init1(table, table->size *2);


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
            rt_resize(table);
        }
        else
        {
            table->worstLookupCount = lookup;
            table->worstLookupCount++;
            if(table->worstLookupCount > RT_MAX_WORSTCASELOOKUPCOUNT)
            {
                rt_resize(table);
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
            free(del);
        }
    }

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


/**************************************************************************************************
 *
 *  REPORT HASH TABLE
 * 
 **************************************************************************************************/


#define RH_HASHTABLE_SIZE_DEFAULT 2048
#define RH_HASH_PRIME 31

#define RH_MAX_WORSTCASELOOKUP 5
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
    table->internal = (rhItemArray *)malloc(sizeof(rhItemArray) * size);
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
    
    int oldsize = table->size;
    rhItemArray *old_data = table->internal;

    rh_init1(table, table->size *2);

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
            rh_resize(table);
        }
        else
        {
            table->worstLookupCount = lookup;
            table->worstLookupCount++;
            if(table->worstLookupCount > RH_MAX_WORSTCASELOOKUPCOUNT)
            {
                rh_resize(table);
            }
        }

    }

    return 0;
}



/****************************************
 * CLEAN UP
 ****************************************/

/**
 * Delete all allocated ram for table
 * This function also deletes ALL key strings!
 */
void rh_clean(reportTable *table)
{
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


/**************************************************************************************************
 *
 *  MAIN OPERATIONS
 * 
 **************************************************************************************************/


/****************************************
 * ENTITIES 
 ****************************************/

/**
 * add a new entity to the hashtable
 * 
 * @param table hastable where to insert
 * @param who entity to add, don't free the allocation, from the call its handled by this function
 * @param who_sz precalculated lenght of who str
 */
static inline void add_entity(hashTable *table, char *who, int who_sz)
{
    if(!ht_hasKey(table, who))
    {
        ht_insert2(table, who, who_sz);
        //DEBUG_PRINT("Added %s\n", command[1]);
    }
    else
    {
        free(who);
    }
}


/**
 * remove a entity from the hashtable
 * 
 * @param table hastable where to remove
 * @param who entity to add, don't free the allocation, from the call its handled by this function
 */
static inline void remove_entity(hashTable *table, relationTable *relations,  char *who)
{
    htItem *rm = ht_hasKey(table, who);
    if(rm)
    {
        rt_removeAll_for(relations, rm);
        ht_remove(table, who);
    }
    
    free(who);
}



/****************************************
 * RELATIONS
 ****************************************/

/**
 * Add a relation if possible
 * 
 * @param entities entities hash table
 * @param relNames relations array
 * @param relations relations hash table
 * @param command source where grab data (source - dest -rel)
 * @param rel_sz lengh of relation str
 */
static inline void add_relation(hashTable *entities, relationArray *relNames, relationTable *relations, char **command, int rel_sz)
{
    htItem *source = ht_hasKey(entities, command[0]);
    htItem *dest = ht_hasKey(entities, command[1]);
    free(command[0]);
    free(command[1]);
    if(source && dest)
    {
        //check if the relation has just been created
        relation *rel = ra_find2(relNames, command[2]);
        if(rel)
        {
            free(command[2]);
        }
        else
        {
            rel = ra_insert(relNames, command[2], rel_sz);
        }

        //insert relelation in rel table
        if(!rt_hasKey(relations, source, dest, rel))
        {
            rt_insert2(relations, source, dest, rel);
        }
    }
    else
    {
        free(command[2]);
    }
}


/**
 * Delete a relation if possible
 * 
 * @param entities entities hash table
 * @param relNames relations array
 * @param relations relations hash table
 * @param command source where grab data (source - dest -rel)
 */
static inline void remove_relation(hashTable *entities, relationArray *relNames, relationTable *relations, char **command)
{
    #ifdef OPERATIONS
    DEBUG_PRINT(">  [Attempt] Delete relation from: %s to: %s rel: %s\n", command[0], command[1], command[2]);
    #endif
    htItem *source = ht_hasKey(entities, command[0]);
    htItem *dest = ht_hasKey(entities, command[1]);
    free(command[0]);
    free(command[1]);
    if(source && dest)
    {
        //check if the relation has just been created
        relation *rel = ra_find2(relNames, command[2]);

        //search and delete relation (not in)
        if(rel)
        {
            rt_remove(relations, source, dest, rel);
        }
        // no relation found

    }
    
    free(command[2]);
}



/****************************************
 * REPORT
 ****************************************/

typedef struct s_top 
{
    htItem *who;
    struct s_top *next;
} Top;


#define REPORT_BUFFER_MIN_SZ 1024

/**
 * Report command
 * 
 * @param entities entities hash table
 * @param relNames relations array
 * @param relations relations hash table
 */
static inline void report(hashTable *entities, relationArray *relNames, relationTable *relations)
{
    // no relations
    if(relNames->size < 1)
    {
        printf("none\n");
        return;
    }

    reportTable *rep = rh_init2();

    int *best_counts = malloc(sizeof(int) * relNames->size);
    memset(best_counts, 0, relNames->size * sizeof(int));

    Top **rel_list = malloc(sizeof(Top**) * relNames->size);
    memset(rel_list, 0, sizeof(Top**) * relNames->size);


    // calculate count for every relation
    int sz = relations->size;
    for(int i = 0; i < sz; i++)
    {
        rtItem *itr = relations->internal[i];
        rhItem *irel;
        while(itr)
        {
            irel = rh_hasKey(rep, itr->to, itr->rel);
            if(irel)
            {
                irel->cout += 1;
                //update best values for relations
                if(best_counts[itr->rel->index] < irel->cout)
                {
                    best_counts[itr->rel->index] = irel->cout;
                }
            }
            else
            {
                rh_insert2(rep, itr->to, itr->rel);
                //update best values for relations
                if(best_counts[itr->rel->index] < 1)
                {
                    best_counts[itr->rel->index] = 1;
                }
            }

            itr = itr->next;
        }
    }

    sz = rep->size;
    for(int i = 0; i < sz; i++)
    {
        rhItem *itr = rep->internal[i];
        while (itr)
        {
            if(itr->cout >= best_counts[itr->rel->index])
            {
                Top *el = malloc(sizeof(Top));
                el->who = itr->to;
                //insert with order
                if(rel_list[itr->rel->index] == NULL || strcmp(el->who->data, rel_list[itr->rel->index]->who->data) < 0)
                {
                    el->next = rel_list[itr->rel->index];
                    rel_list[itr->rel->index] = el;
                }
                else
                {
                    Top *tp = rel_list[itr->rel->index];
                    while(tp)
                    {
                        if(tp->next == NULL || strcmp(el->who->data, tp->next->who->data) < 0)
                        {
                            el->next = tp->next;
                            tp->next = el;
                            break;
                        }
                        tp = tp->next;
                    }
                }
            }

            itr = itr->next;
        }
        
    }

    char *buffer = malloc(REPORT_BUFFER_MIN_SZ);
    int alloc_size = REPORT_BUFFER_MIN_SZ;
    int last_used = 0;

    //print result
    int tlen = 0;
    for(int i = 0; i < relNames->size; i++)
    {
        Top *itr = rel_list[i];
        if(itr)
        {
            
            if(last_used) 
            {
                buffer[last_used] = ' ';
                last_used++;
                //printf(" ");
            }

            tlen = relNames->relations[i]->len;
            if(last_used + tlen > alloc_size)
            {
                alloc_size += REPORT_BUFFER_MIN_SZ;
                buffer = realloc(buffer, alloc_size);
            }
            memcpy(buffer + last_used, relNames->relations[i]->name, tlen);
            last_used += tlen;

            //printf("%s",relNames->relations[i]->name);
            while (itr)
            {
                tlen = itr->who->len;
                if(last_used + tlen + 1 > alloc_size)
                {
                    alloc_size += REPORT_BUFFER_MIN_SZ;
                    buffer = realloc(buffer, alloc_size);
                }
                buffer[last_used] = ' ';
                memcpy(buffer+last_used+1, itr->who->data, tlen);
                last_used += tlen + 1;
                //printf(" %s",itr->who->data);
                itr = itr->next;
            }

            if(last_used + 9 > alloc_size) // approximate average number size
            {
                alloc_size += REPORT_BUFFER_MIN_SZ;
                buffer = realloc(buffer, alloc_size);
            }
            int n = sprintf(buffer+last_used, " %d;", best_counts[i]);
            last_used += n;
            //printf(" %d", best_counts[i]);
        }
    }


    if(!last_used)
        printf("none\n");
    else
        printf("%s\n",buffer);

    //cleanup
    free(best_counts);
    free(buffer);
    Top* itr, *del;
    for(int i = 0; i < relNames->size; i++)
    {
        itr = rel_list[i];
        while (itr)
        {
            del = itr;
            itr = itr->next;
            free(del);
        }
    }
    free(rel_list);
    rh_clean(rep);

}


/****************************************
 * MAIN
 ****************************************/

#define INPUT_BUFFER_SIZE 2048

int main(int argc, char** argv)
{

    //init input data
    char *command[3];

    int exit_loop = 0;

    //init entity table
    hashTable *entities_table;
    entities_table = ht_init2();

    //init relation array
    relationArray relation_names;
    ra_init(&relation_names);

    //init relation table
    relationTable *relation_table;
    relation_table = rt_init2();

    char *buffer = malloc(sizeof(char) * INPUT_BUFFER_SIZE);
    
    /*
        INPUT
    */

    do
    {   
        size_t max_sz = INPUT_BUFFER_SIZE;
        size_t rsz = getline(&buffer, &max_sz, stdin);

        if(buffer[0] == 'a')
        {
            if(buffer[3] == 'e')
            {
                //addent <ent>
                command[0] = malloc(rsz-7);
                memcpy(command[0], (buffer + 7), rsz-8);
                command[0][rsz-8] = '\0';
                add_entity(entities_table, command[0], rsz-8);
            }
            else if(buffer[3] == 'r')
            {
                //addrel <from> <to> <rel>
                int spaces = 0;
                int last_space = 6; //position of the first space
                for(int i = 7; i < rsz && spaces < 2; i++)
                {
                    if(buffer[i] == ' ')
                    {
                        command[spaces] =  malloc(i-last_space);
                        memcpy(command[spaces], buffer + last_space + 1, i - last_space -1 );
                        command[spaces][i-last_space-1] = '\0';
                        last_space = i;
                        spaces++;
                    }
                }
                command[2] =  malloc(rsz - last_space-1);
                memcpy(command[2], (buffer + last_space+1), rsz-last_space-2);
                command[2][rsz-last_space-2] = '\0';
                add_relation(entities_table, &relation_names, relation_table, command, rsz-last_space-2);
            }
        }
        else if(buffer[0] == 'd')
        {
            if(buffer[3] == 'e')
            {
                //delent <ent>
                command[0] = malloc(rsz-7);
                memcpy(command[0], (buffer + 7), rsz-8);
                command[0][rsz-8] = '\0';
                remove_entity(entities_table, relation_table, command[0]);
            }
            else if(buffer[3] == 'r')
            {
                //delrel <from> <to> <rel>
                int spaces = 0;
                int last_space = 6; //position of the first space
                for(int i = 7; i < rsz && spaces < 2; i++)
                {
                    if(buffer[i] == ' ')
                    {
                        command[spaces] =  malloc(i-last_space);
                        memcpy(command[spaces], buffer + last_space + 1, i - last_space -1 );
                        command[spaces][i-last_space-1] = '\0';
                        last_space = i;
                        spaces++;
                    }
                }
                command[2] =  malloc(rsz - last_space-1);
                memcpy(command[2], (buffer + last_space+1), rsz-last_space-2);
                command[2][rsz-last_space-2] = '\0';

                remove_relation(entities_table, &relation_names, relation_table, command);
            }
        }
        else if(buffer[0] == 'r')
        {
            report(entities_table, &relation_names, relation_table);
        }
        else
        {
            //end 
            exit_loop = 1;
        }

        //free(command[0]);

    } while (!exit_loop);
    

    #ifdef DEBUG
    DEBUG_PRINT("\nEntities:\n");
    ht_print_status(entities_table);

    DEBUG_PRINT("\nRelations:\n");
    rt_print_status(relation_table);
    #endif

    //clean relations
    rt_clean(relation_table);
    //rm relation names
    ra_clean(&relation_names);
    //rm entities
    ht_clean(entities_table);

    //rm buffer
    free(buffer);

    return 0;
}