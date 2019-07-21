
typedef struct s_EntityNode
{
    char *data;
    int relations;
    char color;
    struct s_EntityNode *parent, *right, *left;
} EntityNode;


typedef struct
{
    EntityNode *root;
} EntityTree;


static EntityNode *et_liear_stack[30];

EntityTree *et_init()
{
    EntityTree *t = malloc(sizeof(EntityTree));
    t->root = NULL;
    return t;
}

static EntityNode et_sentinel = {0, 0, 0, &et_sentinel, &et_sentinel};


static inline void et_leftRotation(EntityTree *tree, EntityNode *x)
{
    EntityNode *y = x->right;
    x->right = y->left;
    if (y->left != &et_sentinel)
        y->left->parent = x;
    if (y != &et_sentinel)
        y->parent = x->parent;
    if (x->parent)
    {
        if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    }
    else
    {
        tree->root = y;
    }
    y->left = x;
    if (x != &et_sentinel)
        x->parent = y;
}


static inline void et_rightRotation(EntityTree *tree, EntityNode *x)
{
    EntityNode *y = x->left;
    x->left = y->right;
    if (y->right != &et_sentinel)
        y->right->parent = x;
    if (y != &et_sentinel)
        y->parent = x->parent;
    if (x->parent)
    {
        if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    }
    else
    {
        tree->root = y;
    }
    y->right = x;
    if (x != &et_sentinel)
        x->parent = y;
}


static inline void et_insertFix(EntityTree *tree, EntityNode *x)
{
    EntityNode *y;
    while (x != tree->root && x->parent->color == 1)
    {
        if (x->parent == x->parent->parent->left)
        {
            y = x->parent->parent->right;
            if (y->color == 1)
            {
                x->parent->color = 0;
                y->color = 0;
                x->parent->parent->color = 1;
                x = x->parent->parent;
            }
            else
            {
                if (x == x->parent->right)
                {
                    x = x->parent;
                    et_leftRotation(tree, x);
                }
                x->parent->color = 0;
                x->parent->parent->color = 1;
                et_rightRotation(tree, x->parent->parent);
            }
        }
        else
        {
            y = x->parent->parent->left;
            if (y->color == 1)
            {
                x->parent->color = 0;
                y->color = 0;
                x->parent->parent->color = 1;
                x = x->parent->parent;
            }
            else
            {
                if (x == x->parent->left)
                {
                    x = x->parent;
                    et_rightRotation(tree, x);
                }
                x->parent->color = 0;
                x->parent->parent->color = 1;
                et_leftRotation(tree, x->parent->parent);
            }
        }
    }
    tree->root->color = 0;
}


EntityNode *et_insert(EntityTree *tree, char *entity, int *inserted)
{
    int cmp = 0;
    EntityNode *parent = NULL, *itr = tree->root;
    while (itr && itr != &et_sentinel)
    {
        cmp = strcmp(entity, itr->data);
        if (cmp == 0)
        {
            *inserted = 0;
            return itr;
        }
        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;
    }
    EntityNode *node = malloc(sizeof(EntityNode));
    node->data = entity;
    node->color = 1;
    node->left = &et_sentinel;
    node->right = &et_sentinel;
    node->parent = parent;
    node->relations = 0;
    if (parent)
    {
        if (cmp > 0)
            parent->right = node;
        else
            parent->left = node;
    }
    else
    {
        tree->root = node;
    }
    et_insertFix(tree, node);
    *inserted = 1;
    return node;
}


EntityNode *et_search(EntityTree *tree, char *entity)
{
    EntityNode *itr = tree->root;

    while (itr && itr != &et_sentinel)
    {
        int cmp = strcmp(entity, itr->data);
        if (cmp == 0)
            break;
        else
            itr = (cmp > 0) ? itr->right : itr->left;
    }
    return itr != &et_sentinel ? itr : NULL;
}
EntityNode *et_treeMin(EntityNode *tree)
{
    while (tree->left != &et_sentinel)
        tree = tree->left;
    return tree;
}


static inline void et_deleteFix(EntityTree *tree, EntityNode *x)
{
    EntityNode *w;
    while (x != tree->root && x->color == 0)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            if (w->color == 1)
            {
                w->color = 0;
                x->parent->color = 1;
                et_leftRotation(tree, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == 0 && w->right->color == 0)
            {
                w->color = 1;
                x = x->parent;
            }
            else
            {
                if (w->right->color == 0)
                {
                    w->left->color = 0;
                    w->color = 1;
                    et_rightRotation(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->right->color = 0;
                et_leftRotation(tree, x->parent);
                x = tree->root;
            }
        }
        else
        {
            w = x->parent->left;
            if (w->color == 1)
            {
                w->color = 0;
                x->parent->color = 1;
                et_rightRotation(tree, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == 0 && w->left->color == 0)
            {
                w->color = 1;
                x = x->parent;
            }
            else
            {
                if (w->left->color == 0)
                {
                    w->right->color = 0;
                    w->color = 1;
                    et_leftRotation(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->left->color = 0;
                et_rightRotation(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = 0;
}


void et_delete(EntityTree *tree, EntityNode *z)
{
    if (!z)
        return;
    EntityNode *x, *y;
    if (z->left == &et_sentinel || z->right == &et_sentinel)
    {
        y = z;
    }
    else
    {
        y = et_treeMin(z->right);
    }
    if (y->left != &et_sentinel)
        x = y->left;
    else
        x = y->right;
    x->parent = y->parent;
    if (y->parent)
    {
        if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
    }
    else
    {
        tree->root = x;
    }
    if (y != z)
    {
        char *temp = z->data;
        z->data = y->data;
        y->data = temp;
        z->relations = y->relations;
    }
    if (y->color == 0)
        et_deleteFix(tree, x);
    free(y->data);
    free(y);
    if (tree->root == &et_sentinel)
        tree->root = NULL;
}


void et_clean(EntityTree *tree)
{
    int used = 1;
    et_liear_stack[0] = tree->root;
    if (!et_liear_stack[0])
        return;
    EntityNode *p;
    while (used > 0)
    {
        p = et_liear_stack[used - 1];
        used--;
        if (p->right != &et_sentinel)
        {
            et_liear_stack[used] = p->right;
            used++;
        }
        if (p->left != &et_sentinel)
        {
            et_liear_stack[used] = p->left;
            used++;
        }
        free(p->data);
        free(p);
    }
}


void et_count(EntityTree *tree)
{
    int count = 0;
    int used = 1;
    et_liear_stack[0] = tree->root;
    EntityNode *p;
    while (used > 0)
    {
        p = et_liear_stack[used - 1];
        used--;
        if (p->right != &et_sentinel)
        {
            et_liear_stack[used] = p->right;
            used++;
        }
        if (p->left != &et_sentinel)
        {
            et_liear_stack[used] = p->left;
            used++;
        }
        count++;
    }
    printf("Tree elements: %d\n", count);
}
