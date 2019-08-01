
typedef struct s_rcontainer
{
    RelationHolder in;
    RelationHolder out;

} RelationContainer;


/**
 * Assign container if not allocated
 * @param ent
 */
void rc_assign_relation_container(EntityData *ent)
{
    if(ent->rel_container)
        return;
    ent->rel_container = malloc(sizeof(RelationContainer));
    memset(ent->rel_container, 0, sizeof(RelationContainer));
}



int rc_make_relation(EntityData *from, EntityData* to, RelationID relID)
{
    rc_assign_relation_container(from);
    rc_assign_relation_container(to);

    if(rh_insert(&from->rel_container->out, to, relID))
    {
        rh_insert(&to->rel_container->in, from, relID);
        return 1; // inserted new rel
    }

    return 0; // duplicate

}

int rc_delete_relation(EntityData *from, char *to, RelationID relID)
{
    EntityData *del = rh_remove(&from->rel_container->out, to, relID);
    if(del)
    {
        //rel exists
        rh_remove(&del->rel_container->in, from->name, relID);

        return 1;
    }

    return 0;
}


/**
 * Delete all relations for an entity (this also clears internal items)
 * @param ent
 */
void rc_delete_all_for(EntityData *ent)
{
    //remove in rels (of current)
    for(int i = 0; i < SUPPORTED_RELATIONS; i++)
    {
        EntityData **arr = ent->rel_container->in.rels[i];

        for(int j =0; j < ent->rel_container->in.sizes[i]; j++)
        {
            rh_remove(&arr[j]->rel_container->out, ent->name, i);
        }

        if(arr)
            free(arr);
    }

    //remove out rels
    for(int i = 0; i < SUPPORTED_RELATIONS; i++)
    {
        EntityData **arr = ent->rel_container->out.rels[i];

        for(int j =0; j < ent->rel_container->out.sizes[i]; j++)
        {
            rh_remove(&arr[j]->rel_container->in, ent->name, i);
        }

        if(arr)
            free(arr);
    }

    free(ent->rel_container);
    ent->rel_container = NULL;

    //skip remove of current object items to speed up things :#
}


void rc_clean(EntityData *ent)
{
    rh_clean(&ent->rel_container->in);
    rh_clean(&ent->rel_container->out);

    free(ent->rel_container);
    ent->rel_container = NULL;
}