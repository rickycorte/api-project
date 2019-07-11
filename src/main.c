#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

#define INPUT_BUFFER_SIZE 1024

#ifdef DEBUG
    #define DEBUG_PRINT printf
#else
    #define DEBUG_PRINT(...)
#endif

#include "bench.c"

//#include "entity_tree.h"

#include "dio_cane.h"

static inline char *et_allocate(char *data) { return data; }
static inline int et_compare(char* x, char *y) { return  strcmp(x, y); }
static inline void et_deallocate(char *data) {  free(data); }

MAKE_TREE(et, Entity, char *, char *, et_allocate, et_compare, et_deallocate, 0)

/****************************************
 * MAIN
 ****************************************/

int main(int argc, char** argv)
{

    EntityTree *entities = et_init();



    #ifdef DEBUG
    double start_tm = ns();
    #endif

    //init
    char *command[3];
    char *buffer = malloc(sizeof(char) * INPUT_BUFFER_SIZE);

    int exit_loop = 0;


    #ifdef DEBUG
    FILE *fl = fopen("test.txt","r");
    if(!fl) fl= stdin;
    #else
    FILE *fl = stdin
    #endif
    
    /*
     *   INPUT
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

                int res;
                et_insert(entities, command[0], &res);
                if(!res)
                {
                    free(command[0]);
                }

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

                et_delete(entities, command[0]);
                free(command[0]);
                

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

                
            }
        }
        else if(buffer[0] == 'r')
        {
            //report

        }
        else
        {
            //end 
            exit_loop = 1;
        }

    } while (!exit_loop);
    
    et_count(entities);

    //rm buffer
    free(buffer);

    et_clean(entities);
    free(entities);

    #ifdef DEBUG
    if(fl) fclose(fl);

    double msTm = (ns() - start_tm)/1000000;
    printf("\nExecution time: %.2fms ~ %.2fs (%.2fns)\n", msTm, msTm/1000, ns() - start_tm);
    #endif

    return 0;
}