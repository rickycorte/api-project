#include <stdio.h>

#define DEBUG

#ifdef DEBUG
# define DEBUG_PRINT printf 
# define READ_FROM_FILE
#else
# define DEBUG_PRINT(...)
#endif


#include "hashtable.c"


int main(int argc, char** argv)
{

    DEBUG_PRINT("%d\n",ht_hash("ciao", HT_HASHTABLE_SIZE_DEFAULT));
    DEBUG_PRINT("%d\n",ht_hash("se", HT_HASHTABLE_SIZE_DEFAULT));
    DEBUG_PRINT("%d\n",ht_hash("questa", HT_HASHTABLE_SIZE_DEFAULT));
    DEBUG_PRINT("%d\n",ht_hash("oaic", HT_HASHTABLE_SIZE_DEFAULT));
    DEBUG_PRINT("%d\n",ht_hash("di", HT_HASHTABLE_SIZE_DEFAULT));
    DEBUG_PRINT("%d\n",ht_hash("soluzione", HT_HASHTABLE_SIZE_DEFAULT));
    DEBUG_PRINT("%d\n",ht_hash("funziona", HT_HASHTABLE_SIZE_DEFAULT));
    DEBUG_PRINT("%d\n",ht_hash("mandrioli", HT_HASHTABLE_SIZE_DEFAULT));
    DEBUG_PRINT("%d\n",ht_hash("melo", HT_HASHTABLE_SIZE_DEFAULT));
    DEBUG_PRINT("%d\n",ht_hash("succhia", HT_HASHTABLE_SIZE_DEFAULT));
    DEBUG_PRINT("%d\n",ht_hash("a", HT_HASHTABLE_SIZE_DEFAULT));
    DEBUG_PRINT("%d\n",ht_hash("morte", HT_HASHTABLE_SIZE_DEFAULT));
    DEBUG_PRINT("%d\n",ht_hash("cazzo!", HT_HASHTABLE_SIZE_DEFAULT));
    return 0;
}