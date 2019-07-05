#include <stdio.h>

#define DEBUG
#define OPERATIONS

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
    #ifdef DEBUG
    if(!is)
    {
        DEBUG_PRINT("get_formatted_input: Can't read from a null stream\n");
        return 0;
    }
    if(!command)
    {
        DEBUG_PRINT("get_formatted_input: Can't save on a null command array\n");
        return 0;
    }
    #endif

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
                DEBUG_PRINT("get_formatted_input: Broken command, %d max_size exceded\n", max_size);
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
    #ifdef OPERATIONS
    DEBUG_PRINT(">  [Attempt] Create entity: %s\n", command[1]);
    #endif
    if(!ht_hasKey(table, command[1]))
    {
        ht_insert2(table, command[1]);
        //DEBUG_PRINT("Added %s\n", command[1]);
    }
    else
    {
        #ifdef OPERATIONS
        DEBUG_PRINT(" / Duplicate key for %s\n", command[1]);
        #endif
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
    #ifdef OPERATIONS
    DEBUG_PRINT(">  [Attempt] Delete entity: %s\n", command[1]);
    #endif
    htItem *rm = ht_hasKey(table,command[1]);
    if(rm)
    {
        rt_removeAll_for(relations, rm);
        ht_remove(table, command[1]);
    }
    else
    {
        #ifdef OPERATIONS
        DEBUG_PRINT(" / No key for %s\n", command[1]);
        #endif
    }
    //DEBUG_PRINT("Removed %s\n", command[1]);
    
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
    #ifdef OPERATIONS
    DEBUG_PRINT(">  [Attempt] Create relation from: %s to: %s rel: %s\n", command[1], command[2], command[3]);
    #endif
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
    #ifdef OPERATIONS
    DEBUG_PRINT(">  [Attempt] Delete relation from: %s to: %s rel: %s\n", command[1], command[2], command[3]);
    #endif
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

    #ifdef DEBUG
    double start_tm = ns();
    #endif

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

    #ifdef DEBUG
    FILE *fl = fopen("test.txt","r");
    if(!fl) fl= stdin;
    #else
    FILE *fl = stdin
    #endif
    

    do
    {
       if(get_formatted_input(fl, command, &cmd_sz, 4))
       {

/*
            if(strcmp(command[0], "addent") == 0)
            {

            }
            else if(strcmp(command[0], "delent") == 0)
            {

            }
            else if(strcmp(command[0], "addrel") == 0)
            {

            }
            else if(strcmp(command[0], "delrel") == 0)
            {

            }
            else if(strcmp(command[0], "report") == 0)
            {

            }
            else if(strcmp(command[0], "end") == 0)
            {
                exit_loop = 1;

            }
*/

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
            DEBUG_PRINT("%s: command not found\n", command[0]);
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

    #ifdef DEBUG
    DEBUG_PRINT("\nEntities:\n");
    ht_print_status(entities_table);

    DEBUG_PRINT("\nRelations:\n");
    ht_print_status(relation_table);
    #endif

    //clean relations
    rt_clean(relation_table);
    //rm relation names
    ra_clean(&relation_names);
    //rm entities
    ht_clean(entities_table);

    #ifdef DEBUG
    if(fl) fclose(fl);

    double msTm = (ns() - start_tm)/1000000;
    printf("\nExecution time: %.2fms ~ %.2fs\n", msTm, msTm/1000);
    #endif

    return 0;
}