#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

#define INPUT_BUFFER_SIZE 1024
#define REPORT_OUT_QUEUE_SIZE 512
#define REPORT_TREES 50
#define REPORT_BUFFER_SIZE 1024

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



//#include "entity_tree.c"

#include "ent_avl_tree.c"
#include "relation_name_tree.c"
#include "relation_storage_tree.c"
#include "report_tree.c"

/****************************************
 * Delete relations
 ****************************************/

static RelationStorageData **rm_list = NULL;



static inline void remove_all_relations_for(AvlNode *ent, AvlTree *entities, RelationStorageTree *relations, ReportTree *reports[], int rep_count)
{
    static RelationStorageNode *stack[30];

    if(!relations->root)
        return; // no relations

    if(ent->relations > 0)
    {
        stack[0] = relations->root;
        RelationStorageNode *p;

        int stack_used = 1;

        int alloc_sz = 100;
        int used = 0;

        if (!rm_list)
            rm_list = malloc(100 * sizeof(RelationStorageData *));


        while (stack_used > 0) // stack not empty
        {
            p = stack[stack_used - 1];
            stack_used--;

            if (p->right != &rst_sentinel)
            {
                stack[stack_used] = p->right;
                stack_used++;
            }
            if (p->left != &rst_sentinel)
            {
                stack[stack_used] = p->left;
                stack_used++;
            }

            //add nodes to remove list
            if (p->data->from == ent->data || p->data->to == ent->data)
            {
                used++;
                if (used > alloc_sz)
                {
                    alloc_sz += 100;
                    rm_list = realloc(rm_list, alloc_sz * sizeof(RelationStorageData *));
                }

                rm_list[used - 1] = p->data;


                if (p->data->from == ent->data)
                {
                    ReportNode *rep = rep_search(reports[p->data->rel_id], p->data->to);
                    if (rep)
                    {
                        rep->count--;
                        reports[p->data->rel_id]->modified = 1;
                    }

                    AvlNode *dest = avl_search(entities, p->data->to);
                    dest->relations--;

                }

            }

        }

        //delete relations
        for (int i = 0; i < used; i++)
        {
            rst_delete(relations, rm_list[i]->tree_node);
        }
    }

    //delete all reports
    for(int i = 0; i < rep_count; i++)
    {
        ReportNode *rep = rep_search(reports[i], ent->data);
        rep_delete(reports[i], rep);
    }


}


/****************************************
 * Report
 ****************************************/

static char *gb_report_cache[REPORT_TREES] = {0};



static inline int print_rep(char *rel, int rel_id, ReportTree *tree, int space)
{
    static int allocated[REPORT_TREES];
    static int last_used[REPORT_TREES];

    int out_last = 0;

    if(tree->modified)
    {

        static ReportNode *out[REPORT_OUT_QUEUE_SIZE];
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

                //printf(" %s", out[i]->data);

            }

            if(LU + 9 > AP)
            {
                AP += REPORT_BUFFER_SIZE;
                GRCP = realloc(GRCP, AP);
            }
            LU += sprintf(GRCP + LU, " %d;", max);
            //printf(" %d;", max);
        }

    }
    else
    {
        out_last = LU;
    }

    if(LU > 0)
    {
        if(space)
            fwrite(" ", 1, 1, stdout);

        fwrite(GRCP, 1, LU, stdout);

        //printf( space ? " %s" : "%s" , GRCP);
    }

    #undef GRCP
    #undef AP
    #undef LU

    tree->modified = 0;

    return out_last;
}


static inline void report(RelationNameTree *relNames, ReportTree *reports[])
{
    int used = 0;
    static RelationNameNode *stack[20];
    RelationNameNode *curr = relNames->root;

    int print = 0;

    if(curr)
    {

        while(curr != &rel_sentinel || used > 0)
        {
            while(curr != &rel_sentinel)
            {
                stack[used] = curr;
                used++;
                curr = curr->left;
            }

            curr = stack[used-1];
            used--;

            //do shit
            if(reports[curr->id] && reports[curr->id]->root)
            {
                int res = print_rep(curr->data, curr->id, reports[curr->id], print);

                if(res > 0) print = 1;
            }

            curr = curr->right;
        }
    }

    if(!print)
        fwrite("none\n", 1, 5, stdout);
    else
        fwrite("\n", 1, 1, stdout);

}


/****************************************
 * MAIN
 ****************************************/


int main(int argc, char** argv)
{

    AvlTree *entities = avl_init();
    RelationNameTree *relationNames = rel_init();
    RelationStorageTree *relations = rst_init();

    ReportTree *reports[REPORT_TREES] = {0};

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
                command[0] = malloc(rsz-7);
                memcpy(command[0], (buffer + 7), rsz-8);
                command[0][rsz-8] = '\0';

                int res;
                avl_insert(entities, command[0], &res);
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

                // do insertion if possibile
                int res = 0;
                AvlNode *source = avl_search(entities, command[0]);
                if(source)
                {
                    AvlNode *dest = avl_search(entities, command[1]);
                    if(dest)
                    {

                        RelationNameNode *rel =  rel_insert(relationNames, command[2], &res);

                        int r2 = 0;
                        rst_insert(relations, source->data, dest->data, rel->data, rel->id, &r2);


                        if(r2)
                        {
                            source->relations++;
                            dest->relations++;

                            if (!reports[rel->id])
                            {
                                reports[rel->id] = rep_init();
                            }

                            rep_insert(reports[rel->id], dest->data, &r2);
                        }
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
                command[0] = malloc(rsz-7);
                memcpy(command[0], (buffer + 7), rsz-8);
                command[0][rsz-8] = '\0';

                AvlNode *res = avl_search(entities, command[0]);
                if(res)
                {
                    remove_all_relations_for(res, entities, relations, reports, relationNames->count);
                    avl_delete(entities, res);
                }
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

                RelationStorageNode *del = rst_search(relations, command[0], command[1], command[2]);
                if(del)
                {
                    int rel_id = rst_delete(relations, del);

                    AvlNode* dest =  avl_search(entities, command[1]);
                    AvlNode* source = avl_search(entities, command[0]);

                    source->relations--;
                    dest->relations--;

                    ReportNode *rep = rep_search(reports[rel_id], command[1]);
                    if(rep)
                    {
                        rep->count--;
                        reports[rel_id]->modified = 1;
                    }
                    else
                    {
                        DEBUG_PRINT("RM Error: unable to find report data for %s\n", command[1]);
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
            report(relationNames, reports);
        }
        else
        {
            //end 
            exit_loop = 1;
        }

    } while (!exit_loop);


    //rm buffer
    free(buffer);

    avl_clean(entities);
    free(entities);

    rel_clean(relationNames);
    free(relationNames);

    rst_clean(relations);
    free(relations);

    for(int i = 0; i <REPORT_TREES; i++)
    {
        if(reports[i])
        {
            rep_clean(reports[i]);
            free(reports[i]);
        }
        if(gb_report_cache[i])
            free(gb_report_cache[i]);
    }


    if(rm_list)
        free(rm_list);

    #ifdef DEBUG
    if(fl) fclose(fl);

    double msTm = (ns() - start_tm)/1000000;
    printf("\nExecution time: %.2fms ~ %.2fs (%.2fns)\n", msTm, msTm/1000, ns() - start_tm);
    #endif

    return 0;
}