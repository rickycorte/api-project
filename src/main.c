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

#include "rbtree_template.h"

#ifdef DEBUG
    #define FORCE_INLINE __attribute__((always_inline))
#else
#define FORCE_INLINE inline
#endif

#define COMMA ,

typedef struct s_reportlink
{
    char *to;
    int count;
    struct s_reportlink *prev, *next;
} ReportLink;


#include "entitys_tree.c"
#include "relation_names_tree.c"
#include "relation_storage_tree.c"


static inline void remove_all_relations_for(EntityNode *ent, RelationStorageTree *relations, RelationNameTree *relNames, EntityTree *entities)
{
    static RelationStorageNode *stack[30];

    if(!relations->root)
        return; // no relations

    stack[0] = relations->root;
    RelationStorageNode *p;

    int stack_used = 1;

    int alloc_sz = 100;
    int used = 0;
    RelationStorageData **rm_list = malloc(100 * sizeof(RelationStorageData *));

    while(stack_used > 0) // stack not empty
    {
        p = stack[stack_used-1];
        stack_used--;

        if(p->right != &rst_sentinel)
        {
            stack[stack_used] = p->right;
            stack_used++;
        }
        if(p->left != &rst_sentinel)
        {
            stack[stack_used] = p->left;
            stack_used++;
        }

        //add nodes to remove list
        if(p->data->to == ent->data)
        {
            used++;
            if(used > alloc_sz)
            {
                alloc_sz += 100;
                rm_list = realloc(rm_list, alloc_sz *sizeof(RelationStorageData *));
            }

            rm_list[used-1] = p->data;

            if(ent->rank_sz > 0)
            {
                //delete ranks
                //TODO: optimize this shit
                RelationNameNode *rel = rel_search(relNames, p->data->rel);

                if(ent->ranks[rel->id])
                {
                    rel_delete_rank(rel, ent->ranks[rel->id]);
                    ent->ranks[rel->id] = NULL;
                }
            }

        }
        else if (p->data->from == ent->data)
        {
            used++;
            if(used > alloc_sz)
            {
                alloc_sz += 100;
                rm_list = realloc(rm_list, alloc_sz *sizeof(RelationStorageData *));
            }

            rm_list[used-1] = p->data;

            //update ranks for other
            //TODO: optimize this shit
            EntityNode * dest = et_search(entities, p->data->to);
            RelationNameNode *rel = rel_search(relNames, p->data->rel);
            rel_update_rank(rel, dest->ranks[rel->id], -1);

            //printf("%s -1 to %s\n", rel->data, dest->data);
        }
    }

    free(ent->ranks);

    //delete
    for(int i =0; i < used; i++)
    {
        rst_delete(relations, rm_list[i]->tree_node);
    }

    free(rm_list);
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
                        rst_insert(relations, source->data, dest->data, rel->data, &r2);

                        if(r2)
                        {

                            //create/update ranks for dest
                            if (rel->id > dest->rank_sz - 1)
                            {
                                int add = (rel->id / 10 + 1) * 10;
                                dest->ranks = realloc(dest->ranks, (dest->rank_sz + add) * sizeof(ReportLink *));
                                memset((dest->ranks + dest->rank_sz), 0, add * sizeof(ReportLink *));
                                dest->rank_sz += add;
                            }

                            if ((dest->ranks[rel->id]))
                            {
                                rel_update_rank(rel, dest->ranks[rel->id], 1);
                            }
                            else
                            {
                                dest->ranks[rel->id] = rel_make_rank(rel, dest->data);
                            }

                            //printf("%s +1 to %s\n", rel->data, dest->data);
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

                EntityNode *res = et_search(entities, command[0]);
                if(res)
                {
                    // clean all ranks for res

                    remove_all_relations_for(res, relations, relationNames, entities);
                    et_delete(entities, res);
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


                // do deletion if possibile

                RelationStorageNode *del = rst_search(relations, command[0], command[1], command[2]);
                if(del)
                {
                    EntityNode *dest = et_search(entities, command[1]);
                    RelationNameNode *rel = rel_search(relationNames, command[2]);

                    rel_update_rank(rel, dest->ranks[rel->id], -1);
                    //printf("%s -1 to %s\n", rel->data, dest->data);

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

    #ifdef DEBUG
    if(fl) fclose(fl);

    double msTm = (ns() - start_tm)/1000000;
    printf("\nExecution time: %.2fms ~ %.2fs (%.2fns)\n", msTm, msTm/1000, ns() - start_tm);
    #endif

    return 0;
}