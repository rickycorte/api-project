#include <stdio.h>

#ifdef DEBUG
# define DEBUG_PRINT printf 
# define READ_FROM_FILE
#else
# define DEBUG_PRINT(...)
#endif




int main(int argc, char** argv)
{
    return 0;
}