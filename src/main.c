#include <stdio.h>

#define DEBUG

#ifdef DEBUG
# define DEBUG_PRINT printf 
# define READ_FROM_FILE
#else
# define DEBUG_PRINT(...)
#endif


#include "hashtable.c"
#include "bench.c"


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
        DEBUG_PRINT("get_formatted_input: Can't read from a null stream");
        return 0;
    }
    if(!command)
    {
        DEBUG_PRINT("get_formatted_input: Can't save on a null command array");
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
                DEBUG_PRINT("get_formatted_input: Broken command, %d max_size exceded", max_size);
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
        //DEBUG_PRINT("Added %s\n", command[1]);
    }
    else
    {
        //DEBUG_PRINT("Duplicate key for %s\n", command[1]);
        free(command[1]);
    }
}


/**
 * remove a entity from the hashtable
 * 
 * @param table hastable where to remove
 * @param command source where grab data (unused parts are freed except command[0])
 */
static inline void remove_entity(hashTable *table, char **command)
{
    ht_remove(table, command[1]);
    //DEBUG_PRINT("Removed %s\n", command[1]);
    
    //TODO: add relation delete
    free(command[1]);
}



int main(int argc, char** argv)
{
    char *command[4];
    int cmd_sz = 0;

    int exit_loop = 0;

    #ifdef DEBUG
    double start_tm = ns();
    #endif

    hashTable *entities_table;
    entities_table = ht_init2(entities_table);

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
            }

        } else if(command[0][0] == 'd')
        {
            if(command[0][3] == 'e')
            {
                //delent
                remove_entity(entities_table, command);
            }
            else if(command[0][3] == 'r')
            {
                //delrel
            }
        }
        else if(command[0][0] == 'r')
        {

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

    DEBUG_PRINT("\n\n");
    ht_print_status(entities_table);

    ht_clean(entities_table);

    #ifdef DEBUG
    if(fl) fclose(fl);

    double msTm = (ns() - start_tm)/1000000;
    printf("Execution time: %.2fms ~ %.2fs\n", msTm, msTm/1000);
    #endif

    return 0;
}