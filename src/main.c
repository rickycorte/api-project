#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rb_tree.c"
#include "sorted_array.c"

#define DEBUG

#define INPUT_BUFFER_SIZE 512
#ifdef DEBUG
    #define MAX_RELATIONS_IDS 32
#else
    #define MAX_RELATIONS_IDS 5
#endif



#ifdef DEBUG
    #define DEBUG_PRINT printf
#else
    #define DEBUG_PRINT(...)
#endif

#ifdef DEBUG
    #include "bench.c"
#endif

#ifdef DEBUG
    #define FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define FORCE_INLINE inline
#endif

/**
 * Wrapper code in macro to keep code clean!
 * NOT USABLE IN RANDOM PARTE
 */
#define GET_SINGLE_COMMAND_PARAM                                        \
command[0] = malloc(rsz-7);                                             \
memcpy(command[0], (buffer + 7), rsz-8);                                \
command[0][rsz-8] = '\0';

/**
 * Wrapper code in macro to keep code clean!
 * NOT USABLE IN RANDOM PARTE
 */
#define GET_FULL_COMMAND_PARAM                                          \
int spaces = 0;                                                         \
int last_space = 6;                                                     \
for(int i = 7; i < rsz && spaces < 2; i++)                              \
{                                                                       \
if(buffer[i] == ' ')                                                    \
{                                                                       \
command[spaces] =  malloc(i-last_space);                                \
memcpy(command[spaces], buffer + last_space + 1, i - last_space -1 );   \
command[spaces][i-last_space-1] = '\0';                                 \
last_space = i;                                                         \
spaces++;                                                               \
}                                                                       \
}                                                                       \
command[2] =  malloc(rsz - last_space-1);                               \
memcpy(command[2], (buffer + last_space+1), rsz-last_space-2);          \
command[2][rsz-last_space-2] = '\0';


/****************************************
 *
 * RELATIONS
 *
 ****************************************/

typedef struct
{
    char *name;
    size_t id;
} RelationIdentifier;

static inline void *relid_allocator(void *data)
{
    RelationIdentifier *rid = malloc(sizeof(RelationIdentifier));
    rid->id = -1;
    rid->name = data;
    return rid;
}

static inline int relid_comparator(void *src, void *itr_data)
{
    return strcmp(src, ((RelationIdentifier *)itr_data)->name);
}

static inline RelationIdentifier *rel_insert(SortedArray *relationIDS, char *rel)
{
    RelationIdentifier *r = sa_search2(relationIDS, rel);
    if(!r)
        r = sa_insert(relationIDS, rel);

    if(r->id < 0) r->id = relationIDS->size -1;

    return r;
}




/****************************************
 *
 * ENTITIES
 *
 ****************************************/

typedef struct
{
    SortedArray *incoming;
    SortedArray *outgoing;

} RelationData;

typedef struct
{
    char *name;
    RelationData relations[MAX_RELATIONS_IDS];

} EntityData;


static inline void *et_alloc(void * data) {
    EntityData *ed = malloc(sizeof(EntityData));
    ed->name = data;
    memset(ed->relations, 0, sizeof(RelationData) *MAX_RELATIONS_IDS);
    return ed;
}
static inline int et_comp(void * src, void *itr_data) { return strcmp(src, ((EntityData *)itr_data)->name); }

static inline void et_dealloc(void *data) {

    EntityData *ed = (EntityData *)data;
    for(int i=0; i <MAX_RELATIONS_IDS; i++)
    {
        sa_clean(ed->relations[i].incoming);
        sa_clean(ed->relations[i].outgoing);
    }
    free(ed->name);
    free(data);
}


/****************************************
 * MAIN
 ****************************************/

int main(int argc, char** argv)
{
    #ifdef DEBUG
    double start_tm = ns();
    #endif

    //init
    char *command[3];
    char *buffer = malloc(sizeof(char) * INPUT_BUFFER_SIZE);

    int exit_loop = 0;

    RBTree *entities = rb_init(&et_alloc, &et_comp, &et_dealloc);


    #ifdef DEBUG
    FILE *fl = fopen("test.txt","r");
    if(!fl) fl= stdin;
    #else
    FILE *fl = stdin;
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
                GET_SINGLE_COMMAND_PARAM

                int res;

                //insert
                rb_insert(entities, command[0], &res);

                if(!res)
                {
                    free(command[0]);
                }

            }
            else if(buffer[3] == 'r')
            {
                //addrel <from> <to> <rel>
                GET_FULL_COMMAND_PARAM


                // do insertion if possibile
                int res = 0;


                free(command[0]);
                free(command[1]);
                if(!res)
                    free(command[2]);

            }
        }
        else if(buffer[0] == 'd')
        {
            if(buffer[3] == 'e')
            {
                //delent <ent>
                GET_SINGLE_COMMAND_PARAM

                //delete
                rb_delete(entities, rb_search(entities, command[0]));

                free(command[0]);
                

            }
            else if(buffer[3] == 'r')
            {
                //delrel <from> <to> <rel>
                GET_FULL_COMMAND_PARAM


                //insert

                free(command[0]);
                free(command[1]);
                free(command[2]);

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


    //rm buffer
    free(buffer);

    rb_count(entities);

    rb_clean(entities);

    #ifdef DEBUG
    if(fl) fclose(fl);

    double msTm = (ns() - start_tm)/1000000;
    printf("\nExecution time: %.2fms ~ %.2fs (%.2fns)\n", msTm, msTm/1000, ns() - start_tm);
    #endif

    return 0;
}