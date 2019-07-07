#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**************************************************************************************************
 *
 *  ENTITY HASH TABLE
 * 
 **************************************************************************************************/

#define HT_HASHTABLE_SIZE_DEFAULT 37
#define HT_HASH_PRIME 31

#define HT_MAX_WORSTCASELOOKUP 10
#define HT_MAX_WORSTCASELOOKUPCOUNT 20

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
 */
void ht_insert2(hashTable *table, char *key)
{

    htItem *item = (htItem *)malloc(sizeof(htItem));

    item->data = key;

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
 *  RELATIONS ARRAY
 * 
 **************************************************************************************************/

#define RA_DEFAULT_SIZE 10
#define RA_DEFAULT_RESIZE 5


/****************************************
 * DATA STRUCTS
 ****************************************/

typedef struct s_relation
{
    char* name;
    int index;
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
 */
static inline relation *ra_insert(relationArray *arr, char *rel)
{

    relation *irel = malloc(sizeof(relation));
    irel->name = rel;

    //resize if no more space is available
    if(arr->size + 1 > arr->allocated_size)
    {
       relation ** temp = realloc(arr->relations, (arr->allocated_size + RA_DEFAULT_RESIZE) * sizeof(relation**));
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
 *  RELATIONS HASH TABLE
 * 
 **************************************************************************************************/


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
 *  MAIN OPERATIONS
 * 
 **************************************************************************************************/


/****************************************
 * INPUT
 ****************************************/

/**
 * Get formatted input from stream
 * Format specified in minnezza/ProvaFinale2019.pdf (pag 11)
 * Hypothesis: NO FORMAT ERRORS!
 * Note: each element of command is indipendend and can be deallocated at any time without damages
 * 
 * @param is stream where read is done
 * @param command array of char pointers (dim should be 4 to fit biggest command)
 * @param size command item count
 * @param max_size max command size
 * 
 * @return 0 on error, 1 on success
 */
static inline int get_formatted_input(FILE *is, char **command, int *size, int max_size)
{
    char c;

    char *data = (char *)malloc(7);
    int allocated_size = 7;

    int cmd_part = 0;
    int insert_index = 0;

    while ((c = getc(is)) != EOF)
    {
        if(c == ' ')
        {
            //command part is ended, so we store it in cmd array and get ready to read a parameter
            command[cmd_part] = data;
            data[insert_index] = '\0';

            cmd_part++;
            data = NULL;
            allocated_size = 0;
            insert_index = 0;
            continue;

            //TODO: check if i can omit this check
            if(cmd_part > max_size)
            {
                for(int i = 0 ; i < cmd_part; i++)
                {
                    free(command[i]);                   
                }
                free(data);
                return 0;
            }

        }

        if(c == '\n')
        {
            break;
        }

        if(insert_index >= allocated_size-1) // leave last char for terminator
        {
          allocated_size += 5;
          data = (char *)realloc(data, allocated_size);
        }

        data[insert_index] = c;
        insert_index++;
    }

    //final assign at line end
    command[cmd_part] = data;
    data[insert_index] = '\0';
    *size = cmd_part + 1; 
    return 1;
}


/****************************************
 * ENTITIES 
 ****************************************/

/**
 * add a new entity to the hashtable
 * 
 * @param table hastable where to insert
 * @param command source where grab data (unused parts are freed except command[0])
 */
static inline void add_entity(hashTable *table, char **command)
{
    if(!ht_hasKey(table, command[1]))
    {
        ht_insert2(table, command[1]);
    }
    else
    {
        free(command[1]);
    }
}


/**
 * remove a entity from the hashtable
 * 
 * @param table hastable where to remove
 * @param command source where grab data (unused parts are freed except command[0])
 */
static inline void remove_entity(hashTable *table,relationTable *relations, char **command)
{
    htItem *rm = ht_hasKey(table,command[1]);
    if(rm)
    {
        rt_removeAll_for(relations, rm);
        ht_remove(table, command[1]);
    }
    
    free(command[1]);
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
 * @param command source where grab data (unused parts are freed except command[0])
 */
static inline void add_relation(hashTable *entities, relationArray *relNames, relationTable *relations, char **command)
{
    htItem *source = ht_hasKey(entities, command[1]);
    htItem *dest = ht_hasKey(entities, command[2]);
    free(command[1]);
    free(command[2]);
    if(source && dest)
    {
        //check if the relation has just been created
        relation *rel = ra_find2(relNames, command[3]);
        if(rel)
        {
            free(command[3]);
        }
        else
        {
            rel = ra_insert(relNames, command[3]);
        }

        //insert relelation in rel table
        if(!rt_hasKey(relations, source, dest, rel))
        {
            rt_insert2(relations, source, dest, rel);
        }
    }
    else
    {
        free(command[3]);
    }
}


/**
 * Delete a relation if possible
 * 
 * @param entities entities hash table
 * @param relNames relations array
 * @param relations relations hash table
 * @param command source where grab data (unused parts are freed except command[0])
 */
static inline void remove_relation(hashTable *entities, relationArray *relNames, relationTable *relations, char **command)
{
    htItem *source = ht_hasKey(entities, command[1]);
    htItem *dest = ht_hasKey(entities, command[2]);
    free(command[1]);
    free(command[2]);
    if(source && dest)
    {
        //check if the relation has just been created
        relation *rel = ra_find2(relNames, command[3]);

        //search and delete relation (not in)
        if(rel)
        {
            rt_remove(relations, source, dest, rel);
        }
        // no relation found

    }
    
    free(command[3]);
}



/****************************************
 * REPORT
 ****************************************/

typedef struct s_top 
{
    htItem *who;
    struct s_top *next;
} Top;


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

    //best values for relation
    int *best_val = malloc(relNames->size * sizeof(int));
    Top **rel_list = malloc(sizeof(Top**) * relNames->size);

    memset(best_val, 0, relNames->size * sizeof(int));
    memset(rel_list, 0, sizeof(Top**) * relNames->size);

    //reuse allocated a unused items
    Top* pool = NULL;
    
    int *cur_val = malloc(relNames->size * sizeof(int));

    for(int i = 0; i < entities->size; i++)
    {
        htItem *ent = entities->internal[i];
    
        while (ent)
        {
            //iterate all entities
            memset(cur_val, 0, relNames->size * sizeof(int));

            //iterate all relations and count per relation qta of ent
            for(int j = 0; j < relations->size; j++)
            {
                rtItem *rel = relations->internal[j];
            
                while (rel)
                {                
                    if(rel->to == ent)
                    {
                        cur_val[rel->rel->index] += 1;

                        #ifdef REPORT
                        if(!(rel->rel->name) || !(rel->to) || !(rel->to->data) || !(rel->from) || !(rel->from->data)) 
                        {
                            DEBUG_PRINT("OHU NOU");
                        }
                        DEBUG_PRINT("[R] %s +1 to: %s (%p), from %s (%p)\n",
                        rel->rel->name, rel->to->data, rel->to, rel->from->data, rel->from);
                        #endif
                    }

                    rel = rel->next;
                } 

            }

            //check if this ent beats a maximum for a relation
            for(int k = 0; k < relNames->size; k++)
            {
                if(cur_val[k] > best_val[k])
                {
                    best_val[k] = cur_val[k];

                    if(rel_list[k] == NULL)
                    {
                        //allocate a new element (or reuse one in pool)
                        Top *el;
                        if(pool == NULL)
                        {
                           el = malloc(sizeof(Top));
                        }
                        else
                        {
                            el = pool;
                            pool = pool->next;
                        }
                        el->who = ent;
                        el->next = NULL;
                        rel_list[k] = el;
                    }
                    else
                    {
                        rel_list[k]->who = ent; // reuse a present entry
                        //move other allocated items to pool
                        if(rel_list[k]->next != NULL)
                        {
                            //find last element
                            Top *itr = rel_list[k]->next;
                            while (itr->next)
                            {
                                itr = itr->next;
                            }
                            itr->next = pool;
                            pool = rel_list[k]->next;                            
                        }
                        rel_list[k]->next = NULL;
                    }
                }
                //more entitie with same number of relations (not 0)
                else if(cur_val[k] > 0 && cur_val[k] == best_val[k] )
                {
                    //allocate a new element (or reuse one in pool)
                    Top *el;
                    if(pool == NULL)
                    {
                        el = malloc(sizeof(Top));
                    }
                    else
                    {
                        el = pool;
                        pool = pool->next;
                    }

                    el->who = ent;

                    //insert in order
                    if(rel_list[k] && strcmp(el->who->data, rel_list[k]->who->data) < 0)
                    {
                        el->next = rel_list[k];
                        rel_list[k] = el;
                    }
                    else
                    {
                        Top *itr = rel_list[k];
                        while(itr)
                        {
                            if(itr->next == NULL || strcmp(el->who->data, itr->next->who->data) < 0)
                            {
                                el->next = itr->next;
                                itr->next = el;
                                break;
                            }
                            itr = itr->next;
                        }
                    }

                }
                
            }

            ent = ent->next;
        } 

    }

    //print result
    int print = 0;
    for(int i = 0; i < relNames->size; i++)
    {
        Top *itr = rel_list[i];
        if(itr)
        {
            if(print) printf(" ");

            printf("%s", relNames->relations[i]->name);
            print = 1;
            while (itr)
            {
                printf(" %s", itr->who->data);
                itr = itr->next;
            }
            printf(" %d;", best_val[i]);
        }
    }

    if(!print)
        printf("none\n");
    else
            printf("\n");

    //free shit
    free(cur_val);
    Top *del, *itr = pool;
    while (itr)
    {
        del = itr;
        itr = itr->next;
        free(del);
    }
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
    free(best_val);
    free(rel_list);
}


/****************************************
 * MAIN
 ****************************************/

int main(int argc, char** argv)
{
    //init input data
    char *command[4];
    int cmd_sz = 0;

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
    
    do
    {
       if(get_formatted_input(stdin, command, &cmd_sz, 4))
       {
        //assume input is ok
        if(command[0][0] == 'a')
        {
            if(command[0][3] == 'e')
            {
                //addent
                add_entity(entities_table, command);
            }
            else if(command[0][3] == 'r')
            {
                //addrel
                add_relation(entities_table, &relation_names, relation_table, command);
            }

        } else if(command[0][0] == 'd')
        {
            if(command[0][3] == 'e')
            {
                //delent
                remove_entity(entities_table, relation_table, command);
            }
            else if(command[0][3] == 'r')
            {
                //delrel
                 remove_relation(entities_table, &relation_names, relation_table, command);
            }
        }
        else if(command[0][0] == 'r')
        {
            //report
            report(entities_table, &relation_names, relation_table);
        }
        else if(command[0][0] == 'e')
        {         
            exit_loop = 1;
        }
        #ifdef DEBUG
        else
        {
            for(int i =0; i < cmd_sz; i++)
            {
                free(command[i]);
            }
            continue;
        }
        #endif

        free(command[0]);

       }     

    } while (!exit_loop);

    //clean relations
    rt_clean(relation_table);
    //rm relation names
    ra_clean(&relation_names);
    //rm entities
    ht_clean(entities_table);

    return 0;
}