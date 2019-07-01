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
 */
static inline void get_formatted_input(FILE *is, char **command, int *size)
{
    #ifdef DEBUG
    if(!is)
    {
        DEBUG_PRINT("get_formatted_input: Can't read from a null stream");
        return;
    }
    if(!command)
    {
        DEBUG_PRINT("get_formatted_input: Can't save on a null command array");
        return;
    }
    #endif

    char c;

    char *data = (char *)malloc(7);;
    int allocated_size = 7;

    int cmd_part = 0;
    int insert_index = 0;

    while ((c = getc(is)) != EOF)
    {
        if(c == ' ')
        {
            //command part is ended, so we store it in cmd array and get ready to read a parameter
            command[cmd_part] = data;
            data[insert_index+1] = '\0';

            cmd_part++;
            data = NULL;
            allocated_size = 0;
            insert_index = 0;
            continue;
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
    data[insert_index+1] = '\0';
    *size = cmd_part + 1; 
}


int main(int argc, char** argv)
{
    char *command[4];
    int cmd_sz = 0;

    do
    {
       get_formatted_input(stdin, command, &cmd_sz);
       DEBUG_PRINT("Got: ");
       for(int i = 0; i < cmd_sz; i++)
       {
           DEBUG_PRINT("%s ",command[i]);
           if(strcmp(command[0], "end") != 0)
           {
               free(command[i]);
           }
       }
       DEBUG_PRINT("\n");
       

    } while (strcmp(command[0], "end") != 0); 
    for(int i = 0; i < cmd_sz; i++)
    {
        free(command[i]);
    }

    return 0;
}