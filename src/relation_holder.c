

typedef char RelationID;


typedef struct
{
    EntityData **rels[SUPPORTED_RELATIONS];
    short sizes[SUPPORTED_RELATIONS];
    short allocated[SUPPORTED_RELATIONS];
} RelationHolder;


/****************************************
 *
 *  CODE
 *
 ****************************************/

/**
 * Init rtm
 * @return
 */
void rh_init(RelationHolder *rh)
{
    memset(rh, 0, sizeof(RelationHolder));
}

/**
 * Search
 * @param rtm
 * @param relName
 * @return
 */
int rh_searchIDX(RelationHolder *rh, char *other, RelationID relID)
{
    int low =0, height = rh->sizes[relID] -1, mid, cmp;

    while(low <= height)
    {
        mid = (low + height)/2;

        cmp = strcmp(other, rh->rels[relID][mid]->name);

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
EntityData *rh_search(RelationHolder *rh, char *other, RelationID relID)
{
    int sch = rh_searchIDX(rh, other, relID);

    return sch == REL_NOT_FOUND ? NULL : rh->rels[relID][sch];
}


/**
 * Insert with duplicate check (if exist return old ptr)
 * @param rtm
 * @param relName
 * @return 1 if insered new, 0 if duplicate
 */
int rh_insert(RelationHolder *rh, EntityData *other, RelationID relID)
{
    //check duplicate
    EntityData *dup = rh_search(rh, other->name, relID);
    if(dup)
    {
        return 0;
    }

    //check alloc
    if(rh->sizes[relID] + 1 > rh->allocated[relID])
    {
        rh->allocated[relID] += 10;
        rh->rels[relID] = realloc(rh->rels[relID], rh->allocated[relID] * sizeof(EntityData *));
    }

    //insert in order
    int i;
    for(i = rh->sizes[relID] - 1; i >= 0 && strcmp(other->name, rh->rels[relID][i]->name) < 0; i--)
    {
        rh->rels[relID][i+1] = rh->rels[relID][1];
    }

    rh->rels[relID][i+1] = other;

    rh->sizes[relID]++;

    return 1;
}


/**
 * delete relation (not necessary but i test this implementation :#)
 * @param rtm
 * @param relName
 */
EntityData *rh_remove(RelationHolder *rh, char *other, RelationID relID)
{
    int start = rh_searchIDX(rh, other, relID);

    if(start == REL_NOT_FOUND)
        return NULL;

    EntityData *ent = rh->rels[relID][start];

    //delete in order
    for(; start < rh->sizes[relID] - 1; start++)
    {
        rh->rels[relID][start] = rh->rels[relID][start + 1];
    }

    rh->sizes[relID]--;

    //nothing to free ent datas are free by tree
    return ent;
}


/**
 * Delete everything
 */
void rh_clean(RelationHolder *rh)
{
    for (int i = 0; i < SUPPORTED_RELATIONS; i++)
    {
        if(rh->rels[i])
            free(rh->rels[i]);
    }
}