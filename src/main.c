#include <stdio.h>

#define DEBUG
//#define OPERATIONS
//#define REPORT

#define INPUT_BUFFER_SIZE 2048

#ifdef DEBUG
# define DEBUG_PRINT printf 
# define READ_FROM_FILE
#else
# define DEBUG_PRINT(...)
#endif

#include "bench.c"

#include "hashtable.c"
#include "relationArr.c"
#include "relationTable.c"

#include "reportHashTable.c"


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
    #ifdef OPERATIONS
    DEBUG_PRINT(">  [Attempt] Create entity: %s\n", who);
    #endif
    if(!ht_hasKey(table, who))
    {
        ht_insert2(table, who, who_sz);
        //DEBUG_PRINT("Added %s\n", command[1]);
    }
    else
    {
        #ifdef OPERATIONS
        DEBUG_PRINT(" / Duplicate key for %s\n", who);
        #endif
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
    #ifdef OPERATIONS
    DEBUG_PRINT(">  [Attempt] Delete entity: %s\n", who);
    #endif
    htItem *rm = ht_hasKey(table, who);
    if(rm)
    {
        rt_removeAll_for(relations, rm);
        ht_remove(table, who);
    }
    else
    {
        #ifdef OPERATIONS
        DEBUG_PRINT(" / No key for %s\n", who);
        #endif
    }
    //DEBUG_PRINT("Removed %s\n", command[1]);
    
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
    #ifdef OPERATIONS
    DEBUG_PRINT(">  [Attempt] Create relation from: %s to: %s rel: %s\n", command[0], command[1], command[2]);
    #endif
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

int main(int argc, char** argv)
{
    #ifdef DEBUG
    double start_tm = ns();
    #endif

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

    #ifdef DEBUG
    FILE *fl = fopen("test.txt","r");
    if(!fl) fl= stdin;
    #else
    FILE *fl = stdin
    #endif

    char *buffer = malloc(sizeof(char) * INPUT_BUFFER_SIZE);
    
    /*
        INPUT
    */

    do
    {   
        size_t max_sz = INPUT_BUFFER_SIZE;
        size_t rsz = getline(&buffer, &max_sz, fl);

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

    #ifdef DEBUG
    if(fl) fclose(fl);

    double msTm = (ns() - start_tm)/1000000;
    printf("\nExecution time: %.2fms ~ %.2fs (%.2fns)\n", msTm, msTm/1000, ns() - start_tm);
    #endif

    return 0;
}