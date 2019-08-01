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
    #define SUPPORTED_RELATIONS 4
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
#include "report_tree.c"


ReportTree reports[SUPPORTED_RELATIONS]; /* ITS EPIC COLABRODO TIME */


#include "relation_types.c"
#include "relation_holder.c"
#include "relation_container.c"



/****************************************
 * MAIN
 ****************************************/


static char *gb_report_cache[SUPPORTED_RELATIONS] = {0};


static inline int print_rep(char *rel, int rel_id, ReportTree *tree, int space)
{
    static int allocated[SUPPORTED_RELATIONS];
    static int last_used[SUPPORTED_RELATIONS];

    int out_last = 0;

    if(tree->modified)
    {

        static ReportNode *out[REPORT_ELEMENTS_SIZE];
        int max = 1;
        int used = 0;

        static ReportNode *stack[20];
        ReportNode *curr = tree->root;

        if (curr)
        {

            while (curr != &rep_sentinel || used > 0)
            {
                while (curr != &rep_sentinel)
                {
                    stack[used] = curr;
                    used++;
                    curr = curr->left;
                }

                curr = stack[used - 1];
                used--;

                //do shit
                //reset on greater
                if (curr->count > max)
                {
                    out[0] = curr;
                    max = curr->count;
                    out_last = 1;
                } else if (curr->count == max) // append on equal
                {
                    out[out_last] = curr;
                    out_last++;
                }

                curr = curr->right;

            }

        }

        #define GRCP gb_report_cache[rel_id]
        #define AP allocated[rel_id]
        #define LU last_used[rel_id]

        if(!GRCP)
        {
            GRCP = malloc(REPORT_BUFFER_SIZE);
            AP = REPORT_BUFFER_SIZE;
        }

        last_used[rel_id] = 0;

        if (out_last > 0)
        {
            int len = strlen(rel);

            if(LU + len > AP)
            {
                AP += REPORT_BUFFER_SIZE;
                GRCP = realloc(GRCP, AP);
            }

            memcpy(GRCP + LU, rel, len);
            LU += len;

            //printf("%s", rel);

            for (int i = 0; i < out_last; i++)
            {
                len = strlen(out[i]->data); // can optimize

                if(LU + len + 1> AP)
                {
                    AP += REPORT_BUFFER_SIZE;
                    GRCP = realloc(GRCP, AP);
                }

                GRCP[LU] = ' ';
                memcpy(GRCP + 1 + LU, out[i]->data, len);
                LU += len + 1;
            }

            tree->max = max;

            if(LU + 9 > AP)
            {
                AP += REPORT_BUFFER_SIZE;
                GRCP = realloc(GRCP, AP);
            }
            LU += sprintf(GRCP + LU, " %d;", max);
        }
        else
        {
            tree->max = 0;
        }

    }
    else
    {
        out_last = LU;
    }

    if(LU > 0)
    {
        if(space)
            fputs(" ", stdout);

        fwrite(GRCP,1, LU, stdout);

    }

    #undef GRCP
    #undef AP
    #undef LU

    tree->modified = 0;

    return out_last;
}

void report(RelationTypeManager *rtm)
{
    short print = 0;
    RelationID id;

    for(int i =0 ; i < rtm->lastID; i++)
    {
        id = rtm->rels[i].id;
        print += print_rep(rtm->rels[i].name, id, &reports[id], print);
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
    RelationTypeManager *rtm = rtm_init();

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
        #ifdef DEBUG
        LINE++;
        #endif

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
                        RelationType *rel = rtm_insert(rtm, command[2], &res);

                        rc_make_relation(source->data, dest->data, rel->id);

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
                    rc_delete_all_for(res->data);
                    et_delete(entities, res);
                }

                free(command[0]);


            }
            else if(buffer[3] == 'r')
            {
                //delrel <from> <to> <rel>
                GRAB_CMD_0_1_2

                RelationType *rel = rtm_search(rtm, command[2]);
                if(rel)
                {
                    EntityNode *src = et_search(entities, command[0]);

                    if(src)
                    {
                        rc_delete_relation(src->data, command[1], rel->id);
                    }

                }


                free(command[0]);
                free(command[1]);
                free(command[2]);

            }
        }
        else if(buffer[0] == 'r')
        {
            //report
            report(rtm);

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

    rtm_clean(rtm);

    for(int i =0; i < SUPPORTED_RELATIONS; i++)
    {
        rep_clean(&reports[i]);
        if(gb_report_cache[i])
            free(gb_report_cache[i]);
    }

    #ifdef DEBUG
    if(fl) fclose(fl);

    double msTm = (ns() - start_tm)/1000000;
    printf("\nExecution time: %.2fms ~ %.2fs (%.2fns)\n", msTm, msTm/1000, ns() - start_tm);
    #endif

    return 0;
}