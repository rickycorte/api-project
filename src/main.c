#include <stdio.h>

#define DEBUG

#ifdef DEBUG
# define DEBUG_PRINT printf 
# define READ_FROM_FILE
#else
# define DEBUG_PRINT(...)
#endif

#include "rbtree.c"



int main(int argc, char** argv)
{
    rbNode *tree = NULL;
    rb_print(tree);
    for(int i = 10; i > -40; i--)
        rb_Insert(&tree, i);
   
    rb_print(tree);

    return 0;
}