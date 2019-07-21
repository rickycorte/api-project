
//data_type allocator(in_data data)
// int comparator(data_type x, data_type y)
// void deallocator(in_data data)


/**
 * RB tree implamentation definitions
 * pre : function prefix
 * name : tree/node name
 * data_type: internal node data
 * in_data_args : declaration of init/search arguments
 * in_data_call : use of in_data args
 * allocator : data_type allocator( in_data_args ) called with in_data_call
 * comparator : data_type int comparator(data_type, in_data_args ) called with in_data_call (comaprator should inverse the result)
 * deallocator : void deallocator(data_type ) called with in_data_call
 * null_in_data default initializer for data_type
 */
#define MAKE_TREE(pre, name, data_type, in_data_args, in_data_call, allocator, comparator, deallocator, null_in_data)  \
 \
typedef struct s_##name##Node  \
{  \
    data_type data;  \
    int color;  \
    struct s_##name##Node *parent, *right, *left;  \
} name##Node;  \
 \
typedef struct  \
{  \
    name##Node *root;  \
} name##Tree;  \
 \
static name##Node* pre##_liear_stack[30];  \
name##Tree * pre##_init()  \
{  \
    name##Tree *t = malloc(sizeof(name##Tree));  \
    t->root = NULL;  \
 \
    return t;  \
}  \
 static name##Node pre##_sentinel = { null_in_data , 0, 0, & pre##_sentinel, & pre##_sentinel }; \
 \
static inline void  pre##_leftRotation(name##Tree *tree, name##Node *x)  \
{  \
    name##Node *y = x->right;  \
 \
    x->right = y->left;  \
    if(y->left != & pre##_sentinel)  \
        y->left->parent = x; \
 \
    if(y != & pre##_sentinel) y->parent = x->parent; \
    if(x->parent) \
    { \
        if(x == x->parent->right) \
            x->parent->right = y; \
        else \
            x->parent->left = y; \
    } \
    else \
    { \
        tree->root = y; \
    } \
 \
    y->left = x; \
    if(x != & pre##_sentinel) \
        x->parent = y; \
} \
 \
 \
static inline void  pre##_rightRotation(name##Tree *tree, name##Node *x) \
{ \
    name##Node *y = x->left; \
 \
    x->left = y->right; \
    if(y->right != & pre##_sentinel) \
        y->right->parent = x; \
 \
    if(y != & pre##_sentinel) y->parent = x->parent; \
    if(x->parent) \
    { \
        if(x == x->parent->right) \
            x->parent->right = y; \
        else \
            x->parent->left = y; \
    } \
    else \
    { \
        tree->root = y; \
    } \
 \
    y->right = x; \
    if(x != & pre##_sentinel) \
        x->parent = y; \
} \
\
\
\
static inline void  pre##_insertFix(name##Tree *tree, name##Node *x) \
{ \
    name##Node *y; \
\
    while(x != tree->root && x->parent->color == 1) \
    { \
\
        if(x->parent == x->parent->parent->left) \
        { \
            y = x->parent->parent->right; \
 \
            if(y->color == 1) \
            { \
                x->parent->color = 0; \
                y->color = 0; \
                x->parent->parent->color = 1; \
                x = x->parent->parent; \
            } \
            else \
            { \
                if(x == x->parent->right) \
                { \
                    x = x->parent; \
                     pre##_leftRotation(tree, x); \
                } \
 \
                x->parent->color = 0; \
                x->parent->parent->color = 1; \
                 pre##_rightRotation(tree, x->parent->parent); \
            } \
        } \
        else \
        { \
            y = x->parent->parent->left; \
            if(y->color == 1) \
            { \
                x->parent->color = 0; \
                y->color = 0; \
                x->parent->parent->color = 1; \
                x = x->parent->parent; \
            } \
            else \
            { \
                if(x == x->parent->left) \
                { \
                    x = x->parent; \
                     pre##_rightRotation(tree, x); \
                } \
                x->parent->color = 0; \
                x->parent->parent->color = 1; \
\
                 pre##_leftRotation(tree, x->parent->parent); \
            }  \
        }  \
    }  \
    tree->root->color = 0;  \
}  \
 \
 \
 \
name##Node * pre##_insert(name##Tree *tree, in_data_args, int *inserted)  \
{  \
    int cmp = 0; \
    name##Node *parent = NULL, *itr = tree->root; \
 \
    while(itr && itr != & pre##_sentinel) \
    { \
        cmp = comparator (itr->data, in_data_call); \
 \
        if(cmp == 0)  \
        { \
            *inserted = 0;\
            return itr; \
        } \
 \
        parent = itr; \
        itr = (cmp > 0) ? itr->right : itr->left; \
    } \
 \
    name##Node *node = malloc(sizeof(name##Node)); \
    node->data = allocator ( in_data_call ); \
    node->color = 1; \
    node->left = & pre##_sentinel; \
    node->right = & pre##_sentinel; \
    node->parent = parent; \
 \
    if(parent) \
    { \
        if (cmp > 0) \
            parent->right = node; \
        else \
            parent->left = node; \
    } \
    else \
    { \
        tree->root = node; \
    } \
     \
     pre##_insertFix(tree, node); \
    *inserted = 1; \
    return node; \
} \
 \
 \
 \
name##Node * pre##_search(name##Tree *tree, in_data_args) \
{ \
    name##Node *itr = tree->root; \
    while(itr && itr != & pre##_sentinel) \
    { \
        int cmp = comparator (itr->data, in_data_call); \
        if(cmp == 0) \
            break; \
        else \
            itr = (cmp > 0)? itr->right : itr->left; \
    } \
 \
    return itr != & pre##_sentinel ? itr : NULL; \
} \
 \
 \
 \
name##Node * pre##_treeMin(name##Node *tree) \
{ \
    while(tree->left != & pre##_sentinel) \
        tree = tree->left; \
 \
    return tree; \
} \
 \
 \
 \
 \
static inline void  pre##_deleteFix(name##Tree *tree, name##Node *x) \
{ \
    name##Node *w; \
 \
    while(x != tree->root && x->color == 0) \
    { \
 \
        if(x == x->parent->left) \
        { \
            w = x->parent->right; \
 \
            if (w->color == 1) \
            { \
                w->color = 0; \
                x->parent->color = 1; \
 \
                 pre##_leftRotation(tree, x->parent); \
                w = x->parent->right; \
            } \
 \
            if (w->left->color == 0 && w->right->color == 0) \
            { \
                w->color = 1; \
                x = x->parent; \
            } \
            else \
            { \
                if(w->right->color == 0) \
                { \
                    w->left->color = 0; \
                    w->color = 1; \
 \
                     pre##_rightRotation(tree, w); \
                    w = x->parent->right; \
                } \
\
                w->color = x->parent->color; \
                x->parent->color = 0; \
                w->right->color = 0; \
 \
                 pre##_leftRotation(tree, x->parent); \
 \
                x = tree->root; \
            } \
 \
        } \
        else \
        { \
            w = x->parent->left; \
 \
            if (w->color == 1) \
            { \
                w->color = 0; \
                x->parent->color = 1; \
 \
                 pre##_rightRotation(tree, x->parent); \
                w = x->parent->left; \
            } \
 \
            if (w->right->color == 0 && w->left->color == 0) \
            { \
                w->color = 1; \
                x = x->parent; \
            } \
            else \
            { \
                if(w->left->color == 0) \
                { \
                    w->right->color = 0; \
                    w->color = 1; \
 \
                     pre##_leftRotation(tree, w); \
                    w = x->parent->left; \
                } \
 \
                w->color = x->parent->color; \
                x->parent->color = 0; \
                w->left->color = 0; \
 \
                 pre##_rightRotation(tree, x->parent); \
 \
                x = tree->root; \
            } \
        } \
    } \
 \
    x->color = 0; \
} \
 \
 \
void  pre##_delete(name##Tree *tree,  name##Node *z) \
{ \
    if(!z) \
        return; \
 \
    name##Node *x, *y; \
 \
    if(z->left == & pre##_sentinel || z->right == & pre##_sentinel) \
    { \
        y = z; \
    } \
    else \
    { \
        y =  pre##_treeMin(z->right); \
    } \
 \
    if(y->left != & pre##_sentinel) \
        x = y->left; \
    else \
        x = y->right; \
 \
    x->parent = y->parent; \
    if(y->parent) \
    { \
        if(y == y->parent->left) \
            y->parent->left = x; \
        else \
            y->parent->right = x; \
    } \
    else \
    { \
        tree->root = x; \
    } \
\
    if(y != z) \
    { \
        data_type temp = z->data; \
        z->data = y->data; \
        y->data = temp; \
    } \
\
    if(y->color == 0) \
         pre##_deleteFix(tree, x); \
 \
    deallocator (y->data); \
    free(y); \
    if(tree->root == & pre##_sentinel) \
        tree->root = NULL; \
} \
 \
void  pre##_clean(name##Tree *tree) \
{ \
 \
    int used = 1; \
     pre##_liear_stack[0] = tree->root; \
     if(! pre##_liear_stack[0]) return; \
    name##Node *p; \
 \
    while(used > 0) \
    { \
        p =  pre##_liear_stack[used-1]; \
        used--; \
 \
        if(p->right != & pre##_sentinel) \
        { \
             pre##_liear_stack[used] = p->right; \
            used++; \
        } \
        if(p->left != & pre##_sentinel) \
        { \
             pre##_liear_stack[used] = p->left; \
            used++; \
        } \
 \
        deallocator (p->data); \
        free(p); \
 \
    } \
 \
} \
void pre##_count(name##Tree *tree) \
{ \
 \
int count = 0; \
int used = 1; \
pre##_liear_stack[0] = tree->root; \
name##Node *p; \
\
while(used > 0) \
{ \
p = pre##_liear_stack[used-1]; \
used--; \
\
if(p->right != & pre##_sentinel) \
{ \
pre##_liear_stack[used] = p->right; \
used++; \
} \
if(p->left != & pre##_sentinel) \
{ \
pre##_liear_stack[used] = p->left; \
used++; \
} \
\
count++; \
} \
\
printf("Tree elements: %d\n", count); \
\
}
