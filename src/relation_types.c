
#define REL_NOT_FOUND -1

typedef char RelationID;

typedef struct
{
    char *name;
    RelationID id;

} RelationType ;


typedef struct
{
    RelationType rels[SUPPORTED_RELATIONS];
    RelationID lastID; //also used as size
} RelationTypeManager;


/****************************************
 *
 *  CODE
 *
 ****************************************/

/**
 * Init rtm
 * @return
 */
RelationTypeManager *rtm_init()
{
    RelationTypeManager *rtm = malloc(sizeof(RelationTypeManager));
    memset(rtm, 0, sizeof(RelationTypeManager));

    return rtm;
}

/**
 * Search
 * @param rtm
 * @param relName
 * @return
 */
int rtm_searchIDX(RelationTypeManager *rtm, char *relName)
{
    int low =0, height = rtm->lastID -1, mid, cmp;

    while(low <= height)
    {
        mid = (low + height)/2;

        cmp = strcmp(relName, rtm->rels[mid].name);

        if(cmp == 0)
        {
            return mid;
        }
        else if(cmp < 0)
        {
            height = mid - 1;
        }
        else
        {
            low = mid + 1;
        }
    }

    return REL_NOT_FOUND;
}

/**
 * Search wreapper
 * @param rtm
 * @param relName
 * @return
 */
RelationType *rtm_search(RelationTypeManager *rtm, char *relName)
{
    int sch = rtm_searchIDX(rtm, relName);

    return sch == REL_NOT_FOUND ? NULL : &rtm->rels[sch];
}


/**
 * Insert with duplicate check (if exist return old ptr)
 * @param rtm
 * @param relName
 * @return
 */
RelationType *rtm_insert(RelationTypeManager *rtm, char *relName, int *inserted)
{
    //check duplicate
    RelationType *dup = rtm_search(rtm, relName);
    if(dup)
    {
        *inserted = 0;
        return dup;
    }

    int i;
    for(i = rtm->lastID - 1; i >= 0 && strcmp(relName, rtm->rels[i].name) < 0; i--)
    {
        rtm->rels[i+1].name = rtm->rels[i].name;
        rtm->rels[i+1].id = rtm->rels[i].id;
    }

    rtm->rels[i+1].name = relName;
    rtm->rels[i+1].id = rtm->lastID++;

    *inserted = 1;
    return &rtm->rels[i+1];
}


/**
 * delete relation (not necessary but i test this implementation :#)
 * @param rtm
 * @param relName
 */
void rtm_remove(RelationTypeManager *rtm, char *relName)
{
    int start = rtm_searchIDX(rtm, relName);
    if(start == REL_NOT_FOUND)
        return;

    for(; start < rtm->lastID - 1; start++)
    {
        rtm->rels[start].name = rtm->rels[start + 1].name;
        rtm->rels[start].id = rtm->rels[start + 1].id;
    }

    rtm->lastID--;

    //questa cosa fa leak :3
    //bisogna salvarsi il dato tolto per poi eliminarlo se serve :3
}


/**
 * Delete everything
 */
void rtm_clean(RelationTypeManager *rtm)
{
    for (int i = 0; i < rtm->lastID; i++)
    {
        free(rtm->rels[i].name);
    }

    free(rtm);
}