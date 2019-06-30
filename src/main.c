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
    rbTree tree = NULL;
    rb_print(tree);
    for(int i = 10; i > -40; i--)
        rb_Insert(&tree, i);
   
    rb_print(tree);

    for(int i = -30; i < -15; i++ )
    {
        rb_Delete2(&tree, i);
    }

    rb_print(tree);

    for(int i = -40; i < 0; i++ )
    {
        rb_Delete2(&tree, i);
    }

    rb_print(tree);


    return 0;
}