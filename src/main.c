#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

#define INPUT_BUFFER_SIZE 1024
#define REPORT_ELEMENTS_SIZE 512
#define REPORT_BUFFER_SIZE 512

#ifdef DEBUG
    #define SUPPORTED_RELATIONS 32
#else
    #define SUPPORTED_RELATIONS 5
#endif

#ifdef DEBUG
    #define DEBUG_PRINT printf
#else
    #define DEBUG_PRINT(...)
#endif

#ifdef DEBUG
    #include "bench.c"
    #include "rbtree_template.h"
#endif

#ifdef DEBUG
    #define FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define FORCE_INLINE inline
#endif


#define STACK_SIZE 25

/* MAIN ONLY MACROS */

#define GRAB_CMD_0                                                                  \
command[0] = malloc(rsz-7);                                                         \
memcpy(command[0], (buffer + 7), rsz-8);                                            \
command[0][rsz-8] = '\0';


#define GRAB_CMD_0_1_2                                                              \
int spaces = 0;                                                                     \
int last_space = 6;                                                                 \
for(int i = 7; i < rsz && spaces < 2; i++)                                          \
{                                                                                   \
    if(buffer[i] == ' ')                                                            \
    {                                                                               \
        command[spaces] =  malloc(i-last_space);                                    \
        memcpy(command[spaces], buffer + last_space + 1, i - last_space -1 );       \
        command[spaces][i-last_space-1] = '\0';                                     \
        last_space = i;                                                             \
        spaces++;                                                                   \
    }                                                                               \
}                                                                                   \
command[2] =  malloc(rsz - last_space-1);                                           \
memcpy(command[2], (buffer + last_space+1), rsz-last_space-2);                      \
command[2][rsz-last_space-2] = '\0';

/* END MAIN ONLY MACROS END */


#include "entity_tree.c"
#include "relation_name_tree.c"
#include "relation_storage_tree.c"

/****************************************
 * REPORT CACHE
 ****************************************/

typedef struct
{
    char status;                                       // 0 = use cache_str, 1 = elements items changed, 2 = max is not valid, 3 = empty
    unsigned short max;                                     // last max found
    unsigned short last_element;                            // last unused index
    EntityData *elements[REPORT_ELEMENTS_SIZE];             // elements w/ relations == max

    char *cache_str;                                        // cache string calculated in prev reports
    unsigned int cache_sz;                                  // cache string len
    unsigned int cache_allocated;                           // cache string allocated size

} ReportCacheBlock;


static ReportCacheBlock reportCache[SUPPORTED_RELATIONS] = {0};


static inline void add_to_cache(EntityData *ent, int relID)
{
    ReportCacheBlock *rcp = &reportCache[relID];

    if(ent->incoming_rel_count[relID] > rcp->max) // can still recover from a cache clear
    {
        rcp->max = ent->incoming_rel_count[relID];
        rcp->elements[0] = ent;
        rcp->last_element = 1;
        rcp->status = 1;
    }
    else if(rcp->status != 2 && ent->incoming_rel_count[relID] == rcp->max)
    {
        rcp->elements[rcp->last_element] = ent;
        rcp->last_element++;
        rcp->status = 1;
    }
}


static inline void remove_from_cache(RelationStorageData *dl_rel)
{

    int relID = dl_rel->rel->id;

    //clear cache if a max could be changed :L
    if(dl_rel->to->incoming_rel_count[relID] == reportCache[relID].max)
    {
        reportCache[relID].status = 2;
        reportCache[relID].last_element = 0;
    }

}


/****************************************
 * Delete relations
 ****************************************/

static inline void remove_all_relations_for(EntityNode *ent, RelationStorageTree *relations)
{

    if(!relations->root)
        return; // no relations at all

    if(ent->data->relations > 0)
    {

        RelationStorageData *del = NULL, *itr = relations->last;

        while(itr)
        {

            //check for delete + update cache
            if(itr->from == ent->data || itr->to == ent->data)
            {
                del = itr;
                //TODO: update cache
                remove_from_cache(del);
            }


            itr = itr->prev;

            //delete relation
            if(del)
            {
                rst_delete(relations, del->tree_node);
                del = NULL;
            }

        }

    }

}


/****************************************
 * Report
 ****************************************/


static inline int quick_partition(EntityData **arr, int start, int end)
{
    EntityData *x = arr[end];
    int i = start - 1;

    for(int j = start; j < end; j++)
    {
        if(strcmp(arr[j]->name, x->name) <= 0)
        {
            i += 1;
            EntityData *t = arr[i];
            arr[i] = arr[j];
            arr[j] = t;
        }
    }

    EntityData *t = arr[i+1];
    arr[i+1] = arr[end];
    arr[end] = t;

    return i + 1;
}

static inline void quick_sort_internal(EntityData **arr, int start, int end)
{
    if(start < end)
    {
        int q = quick_partition(arr, start, end);
        quick_sort_internal(arr, start, q-1);
        quick_sort_internal(arr, q+1, end);
    }
}



static inline void sort_cache(int relID)
{
    quick_sort_internal(reportCache[relID].elements, 0, reportCache[relID].last_element - 1);
}


static inline void recalculate_max_for_rel(EntityTree *entities, int relID)
{
    ReportCacheBlock *rcp = &reportCache[relID];

    EntityNode *curr = entities->root;
    static EntityNode *stack[STACK_SIZE];
    char used = 0;

    rcp->max = 1;
    rcp->last_element = 0;
    rcp->status = 1;

    //TODO: posso mettere un controllo sul numero di relazioni perdendo un intero per relazione + una somma x aggiunta/rimozione rel

    if(curr)
    {
        while (curr != &et_sentinel || used > 0)
        {
            while (curr != &et_sentinel)
            {
                stack[used] = curr;
                used++;
                curr = curr->left;
            }

            curr = stack[--used];

            //operation
            if(curr->data->incoming_rel_count[relID] > rcp->max)
            {
                rcp->elements[0] = curr->data;
                rcp->last_element = 1;
                rcp->max = curr->data->incoming_rel_count[relID];
            }
            else if (curr->data->incoming_rel_count[relID] == rcp->max)
            {
                rcp->elements[rcp->last_element] = curr->data;
                rcp->last_element++;
            }


            curr = curr->right;
        }
    }

}

#define NUM_SZ 8

void write_max(int num)
{

    //we know max num is below 512 :3

    char b[NUM_SZ];
    short used = 0;

    while (num != 0)
    {
        b[NUM_SZ-used-1] = '0'+ (num % 10);
        used++;
        num /= 10;
    }
    //now we got all number at end

    //move to start
    b[0] = ' ';
    int i;
    for(i = 1; used > 0; used--, i++)
    {
        b[i] = b[NUM_SZ-used];
    }
    b[i] = ';';
    b[i+1] = '\0';

    fputs(b, stdout);
}


static inline int write_rep_block(char *out_buff, int max, char print)
{
    if(out_buff)
    {
        if(print)
            fputs(" ",stdout);

        fputs(out_buff, stdout);

        //printf(" %d;", max); // split max so we can skip cache recalculation
        write_max(max);

        return 1;
    }

    return 0;
}

/**
 * Create report string (require ordered elements!)
 * @param rel_name
 * @param relID
 */
static inline void calculate_str_cache(char *rel_name, int relID)
{
    ReportCacheBlock *rcp = &reportCache[relID];

    if(rcp->cache_allocated <= 0)
    {
        rcp->cache_allocated += REPORT_BUFFER_SIZE;
        rcp->cache_str = malloc( rcp->cache_allocated);
    }

    rcp->cache_sz = 0;

    if(rcp->max < 1 || rcp->last_element  < 1)
    {
        rcp->status = 3;
        rcp->last_element = 0;
        rcp->max = 0;
        return;
    }


    int len = strlen(rel_name);

    memcpy(rcp->cache_str, rel_name, len);
    rcp->cache_sz += len;

    for(int i = 0; i < rcp->last_element; i++)
    {
        len = strlen(rcp->elements[i]->name) + 1; // add space

        if(rcp->cache_sz + len> rcp->cache_allocated)
        {
            rcp->cache_allocated += REPORT_BUFFER_SIZE;
            rcp->cache_str = realloc(rcp->cache_str, rcp->cache_allocated);
        }

        rcp->cache_str[rcp->cache_sz] = ' ';
        memcpy(rcp->cache_str + rcp->cache_sz +1, rcp->elements[i]->name, len -1);

        rcp->cache_sz += len;
    }

    rcp->cache_str[rcp->cache_sz++] = '\0';

    // split max so we can skip cache recalculation if only max of best rel changes
    //rcp->cache_sz += sprintf(rcp->cache_str + rcp->cache_sz, " %d;", rcp->max);

    rcp->status = 0;
}


void report(EntityTree *entities, RelationNameTree *relTypes)
{

    static int cnt = 0;
    cnt++;

    RelationNameNode *curr = relTypes->root;
    static RelationNameNode *stack[STACK_SIZE];
    char used = 0;

    char print = 0;

    if(curr)
    {
        while (curr != &rel_sentinel || used > 0)
        {
            while (curr != &rel_sentinel)
            {
                stack[used] = curr;
                used++;
                curr = curr->left;
            }

            curr = stack[--used];

            //operation
            ReportCacheBlock *rcp = &reportCache[curr->id];

            switch(rcp->status)
            {
                case 0:
                    //just print (done at switch end)
                    break;

                case 1:
                    if(rcp->last_element > 1)
                    {
                        sort_cache(curr->id);
                    }
                    calculate_str_cache(curr->data, curr->id);
                    rcp->status = 0;
                    break;

                case 2:
                    recalculate_max_for_rel(entities, curr->id);
                    calculate_str_cache(curr->data, curr->id);
                    break;
                case 3:
                    //nothing to do here
                    break;
            }

            if(rcp->status == 0)
            {
                print += write_rep_block(rcp->cache_str, rcp->max, print);
            }

            curr = curr->right;
        }
    }

    if(!print)
        fputs("none\n", stdout);
    else
        fputs("\n", stdout);
}


/****************************************
 * MAIN
 ****************************************/

int main(int argc, char** argv)
{

    EntityTree *entities = et_init();
    RelationNameTree *relationNames = rel_init();
    RelationStorageTree *relations = rst_init();

    #ifdef DEBUG
    double start_tm = ns();

    static int LINE = 0;
    #endif

    //init
    char *command[3];
    char *buffer = malloc(sizeof(char) * INPUT_BUFFER_SIZE);

    int exit_loop = 0;


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
        LINE++;
        size_t max_sz = INPUT_BUFFER_SIZE;
        size_t rsz = getline(&buffer, &max_sz, fl);

        if(buffer[0] == 'a')
        {
            if(buffer[3] == 'e')
            {
                //addent <ent>
                GRAB_CMD_0

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

                GRAB_CMD_0_1_2

                // do insertion if possibile
                int res = 0;
                EntityNode *source = et_search(entities, command[0]);
                if(source)
                {
                    EntityNode *dest = et_search(entities, command[1]);
                    if(dest)
                    {

                        RelationNameNode *rel =  rel_insert(relationNames, command[2], &res);

                        int r2 = 0;
                        rst_insert(relations, source->data, dest->data, rel, &r2);

                        //TODO: update maximum cache
                        if(r2)
                            add_to_cache(dest->data, rel->id);

                    }
                }

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
                GRAB_CMD_0

                EntityNode *res = et_search(entities, command[0]);
                if(res)
                {
                    remove_all_relations_for(res, relations);
                    et_delete(entities, res);
                }

                free(command[0]);
                

            }
            else if(buffer[3] == 'r')
            {
                //delrel <from> <to> <rel>
                GRAB_CMD_0_1_2

                RelationStorageNode *del = rst_search(relations, command[0], command[1], command[2]);
                if(del)
                {
                    //TODO: update maxumin cache
                    remove_from_cache(del->data);

                    rst_delete(relations, del);
                }

                free(command[0]);
                free(command[1]);
                free(command[2]);

            }
        }
        else if(buffer[0] == 'r')
        {
            //report
            report(entities, relationNames);
        }
        else
        {
            //end 
            exit_loop = 1;
        }

    } while (!exit_loop);


    //rm buffer
    free(buffer);

    et_clean(entities);
    free(entities);

    rel_clean(relationNames);
    free(relationNames);

    rst_clean(relations);
    free(relations);

    //ahah delete cache here :3

    for(int i=0; i < SUPPORTED_RELATIONS; i++)
    {
        if(reportCache[i].cache_str)
            free(reportCache[i].cache_str);
    }

    
    #ifdef DEBUG
    if(fl) fclose(fl);

    double msTm = (ns() - start_tm)/1000000;
    printf("\nExecution time: %.2fms ~ %.2fs (%.2fns)\n", msTm, msTm/1000, ns() - start_tm);
    #endif

    return 0;
}