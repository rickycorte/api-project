
typedef struct s_ReportNode
{
    char *data;
    int count;
    int color;
    struct s_ReportNode *parent, *right, *left;
} ReportNode;
typedef struct
{
    int modified;
    ReportNode *root;
} ReportTree;
static ReportNode *rep_liear_stack[30];
ReportTree *rep_init()
{
    ReportTree *t = malloc(sizeof(ReportTree));
    t->root = NULL;
    t->modified = 1;
    return t;
}
static ReportNode rep_sentinel = {0, 0, 0, &rep_sentinel, &rep_sentinel};
static inline void rep_leftRotation(ReportTree *tree, ReportNode *x)
{
    ReportNode *y = x->right;
    x->right = y->left;
    if (y->left != &rep_sentinel)
        y->left->parent = x;
    if (y != &rep_sentinel)
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
    if (x != &rep_sentinel)
        x->parent = y;
}
static inline void rep_rightRotation(ReportTree *tree, ReportNode *x)
{
    ReportNode *y = x->left;
    x->left = y->right;
    if (y->right != &rep_sentinel)
        y->right->parent = x;
    if (y != &rep_sentinel)
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
    if (x != &rep_sentinel)
        x->parent = y;
}
static inline void rep_insertFix(ReportTree *tree, ReportNode *x)
{
    ReportNode *y;
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
                    rep_leftRotation(tree, x);
                }
                x->parent->color = 0;
                x->parent->parent->color = 1;
                rep_rightRotation(tree, x->parent->parent);
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
                    rep_rightRotation(tree, x);
                }
                x->parent->color = 0;
                x->parent->parent->color = 1;
                rep_leftRotation(tree, x->parent->parent);
            }
        }
    }
    tree->root->color = 0;
}
ReportNode *rep_insert(ReportTree *tree, char *to, int *inserted)
{
    tree->modified = 1;

    int cmp = 0;
    ReportNode *parent = NULL, *itr = tree->root;
    while (itr && itr != &rep_sentinel)
    {
        cmp = strcmp(to, itr->data);
        if (cmp == 0)
        {
            *inserted = 0;
            itr->count++;
            return itr;
        }
        parent = itr;
        itr = (cmp > 0) ? itr->right : itr->left;
    }
    ReportNode *node = malloc(sizeof(ReportNode));
    node->data = to;
    node->color = 1;
    node->left = &rep_sentinel;
    node->right = &rep_sentinel;
    node->parent = parent;
    node->count = 1; // start at 1 relation

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
    rep_insertFix(tree, node);
    *inserted = 1;
    return node;
}
ReportNode *rep_search(ReportTree *tree, char *to)
{
    ReportNode *itr = tree->root;
    while (itr && itr != &rep_sentinel)
    {
        int cmp = strcmp(to, itr->data);
        if (cmp == 0)
            break;
        else
            itr = (cmp > 0) ? itr->right : itr->left;
    }
    return itr != &rep_sentinel ? itr : NULL;
}
ReportNode *rep_treeMin(ReportNode *tree)
{
    while (tree->left != &rep_sentinel)
        tree = tree->left;
    return tree;
}
static inline void rep_deleteFix(ReportTree *tree, ReportNode *x)
{
    ReportNode *w;
    while (x != tree->root && x->color == 0)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            if (w->color == 1)
            {
                w->color = 0;
                x->parent->color = 1;
                rep_leftRotation(tree, x->parent);
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
                    rep_rightRotation(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->right->color = 0;
                rep_leftRotation(tree, x->parent);
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
                rep_rightRotation(tree, x->parent);
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
                    rep_leftRotation(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->left->color = 0;
                rep_rightRotation(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = 0;
}
void rep_delete(ReportTree *tree, ReportNode *z)
{
    if (!z)
        return;

    tree->modified = 1;

    ReportNode *x, *y;
    if (z->left == &rep_sentinel || z->right == &rep_sentinel)
    {
        y = z;
    }
    else
    {
        y = rep_treeMin(z->right);
    }
    if (y->left != &rep_sentinel)
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
        z->count = y->count;
    }
    if (y->color == 0)
        rep_deleteFix(tree, x);
    free(y);
    if (tree->root == &rep_sentinel)
        tree->root = NULL;
}
void rep_clean(ReportTree *tree)
{
    int used = 1;
    rep_liear_stack[0] = tree->root;
    if (!rep_liear_stack[0])
        return;
    ReportNode *p;
    while (used > 0)
    {
        p = rep_liear_stack[used - 1];
        used--;
        if (p->right != &rep_sentinel)
        {
            rep_liear_stack[used] = p->right;
            used++;
        }
        if (p->left != &rep_sentinel)
        {
            rep_liear_stack[used] = p->left;
            used++;
        }
        free(p);
    }
}
void rep_count(ReportTree *tree)
{
    int count = 0;
    int used = 1;
    rep_liear_stack[0] = tree->root;
    ReportNode *p;
    while (used > 0)
    {
        p = rep_liear_stack[used - 1];
        used--;
        if (p->right != &rep_sentinel)
        {
            rep_liear_stack[used] = p->right;
            used++;
        }
        if (p->left != &rep_sentinel)
        {
            rep_liear_stack[used] = p->left;
            used++;
        }
        count++;
    }
    printf("Tree elements: %d\n", count);
}
