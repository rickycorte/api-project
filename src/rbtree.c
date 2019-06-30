#include <stdlib.h>
#include <stdio.h>


/*
 Comment to input check of functions
*/
#define RB_INPUT_CHECK

enum RB_COLOR {RED, BLACK};

/**
*  Red-Black tree node structure 
*/
typedef struct s_rbNode
{
    int data;
    enum RB_COLOR color;

    struct s_rbNode *left, *right, *parent; 

} rbNode;

typedef rbNode* rbTree;


/*****************************************
 * SIMPLE OPERATIONS
 *****************************************/

rbNode *rb_treeMin(rbTree tree)
{
    while(tree && tree->left)
        tree = tree->left;
    
    return tree;
}

rbNode *rb_treeMax(rbTree tree)
{
    while(tree && tree->right)
        tree = tree->right;
    
    return tree;
}

int rb_node_depth(rbNode *node)
{
    int i = 0;
    for (i = 0; node; i++)
        node = node->parent;

    return i;
}

/*****************************************
 * DEBUG
 *****************************************/

#ifdef DEBUG

    #define KRED  "\x1B[31m"
    #define KGRN  "\x1B[37m"

    void rb_print_sub_tree(rbTree tree, int rb_height)
    {   
        if(!tree)
        {
            // complete graph with nill nodes
            /* if(rb_height > 0)
            {
                rb_print_sub_tree(NULL, rb_height-1);

                for(int i = 0; i < rb_height; i++)
                {
                    printf("     ");
                }
                printf("%sNIL -+\n", KGRN);
                rb_print_sub_tree(NULL, rb_height-1);
                
            } */   

            return;
        }

        rb_print_sub_tree(tree->left, rb_height-1);
        //int depth = rb_node_depth(tree);

        for(int i = 0; i < rb_height; i++)
        {
            printf("          ");
        }
        printf("%s%03d ----+\n", tree->color == RED ? KRED : KGRN, tree->data);
        rb_print_sub_tree(tree->right, rb_height-1);
    }

    void rb_print(rbNode *root)
    {
        if(!root) 
        {
            printf("Empty\n");
            return;
        }

        int rb_height = 0;
        rbNode *node = root;
        // this should find the height
        while(node != NULL)
        {
            rb_height++;
            if(node->left)
                node = node->left;
            else if(node->right) node = node->right;
            else node = NULL;
        }
        printf("rb height (NOT CORRECT) %d\n", rb_height);

        rb_print_sub_tree(root, rb_height-1);
        
    }

    #undef KRED
    #undef KGRN

#endif

/*****************************************
 * ROTATIONS
 *****************************************/

/**
 * Execute left rotation on node
 */
static inline void irb_LeftRotation(rbTree *tree, rbNode *node)
{
    rbNode *y = node->right;
#ifdef DEBUG
    if(!y->right)
        DEBUG_PRINT("IRB_LeftRotation: %d node right can't be NULL\n", node->data);
#endif

    node->right = y->left;
    y->parent = node->parent;

    if(!y->parent)
        *tree = y;
    else if(node->parent->right == node)
        node->parent->right = y;
    else 
        node->parent->left = y;    

    node->parent = y;
    y->left = node;
        
}

/**
 * Execute right rotation on node
 */
static inline void irb_RightRotation(rbTree *tree, rbNode *node)
{
    rbNode *y = node->left;
#ifdef DEBUG
    if(!y->left)
        DEBUG_PRINT("IRB_RightRotation: %d node right can't be NULL\n", node->data);
#endif

    node->left = y->right;
    y->parent = node->parent;

    if(!y->parent)
        *tree = y;
    else if(node->parent->right == node)
        node->parent->right = y;
    else 
        node->parent->left = y;    

    node->parent = y;
    y->right = node;
        
}


/*****************************************
 * INSERTION
 *****************************************/

static inline rbNode* irb_make_node(int value)
{
    rbNode *new_node = (rbNode*)malloc(sizeof(rbNode));
    if(!new_node) return NULL;

    new_node->data = value;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    new_node->color = RED;

    return new_node;
}

/**
 * Fix violations after insertion
 */
static inline void irb_InsertFix(rbTree *tree, rbNode *node)
{
    rbNode *grandParent = NULL, *uncle = NULL;

    while(node != *tree && node->parent->color == RED)
    {
        grandParent = node->parent->parent;

        #ifdef DEBUG
        if(!grandParent)
        {
            DEBUG_PRINT("IRB_InsertFix: Missing grandparent of node %d\n", node->data);
            continue;
        } 
        #endif
        //left child
        if(node->parent == grandParent->left)
        {
            uncle = grandParent->right;
            //red uncle
            //null uncle = black
            if(uncle && uncle->color == RED)
            {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                grandParent->color = RED;
                node = grandParent;
            }
            else
            {
                //black uncle
                if(node == node->parent->right)
                {
                    node = node->parent;
                    irb_LeftRotation(tree, node);
                }

                node->parent->color = BLACK;
                grandParent->color = RED;
                irb_RightRotation(tree, grandParent);
            } 
        }
        else
        {
             uncle = grandParent->left;
            //red uncle
            //null uncle = black
            if(uncle && uncle->color == RED)
            {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                grandParent->color = RED;
                node = grandParent;
            }
            else
            {
                //black uncle
                if(node == node->parent->left)
                {
                    node = node->parent;
                    irb_RightRotation(tree, node);
                }

                node->parent->color = BLACK;
                grandParent->color = RED;
                irb_LeftRotation(tree, grandParent);
            } 
        }

    }

    (*tree)->color = BLACK;
}

/**
 * Insert a new node in the tree
 * 
 * @param tree rb tree to use (can be null)
 * @param value value of the new node
 */
void rb_Insert(rbTree *tree, int value)
{
    #ifdef RB_INPUT_CHECK
    if(!tree)
    {
        DEBUG_PRINT("RB_Insert: Error, must pass a not null tree pointer!\n");
        return;
    }
    #endif

    rbNode *node = irb_make_node(value);
    if(!*tree)
    {
        *tree = node;
        node->color = BLACK;
        return;
    }

    rbNode *itr = *tree, *parent;
    while(itr != NULL)
    {
        parent = itr;
        itr = (value > itr->data)? itr->right : itr->left;
    }
    node->parent = parent;

    if(value > node->data)
        parent->right = node;
    else
        parent->left = node;

    //fix errors
    irb_InsertFix(tree, node);

    DEBUG_PRINT("RB_Insert: Inserted %d\n", value);
}


/*****************************************
 * SEARCH
 *****************************************/

/**
 * Search if a item is in the tree
 * 
 * @param itr tree root, can be NULL
 * @param val value to search
 * @return NULL if not found, pointer to tree element if found
 */
rbNode *rb_Search(rbTree itr, int val)
{
    #ifdef RB_INPUT_CHECK
    if(!itr)
    {
        DEBUG_PRINT("RB_Search: Can't search a null tree!\n");
        return NULL;
    }
    #endif

    while(itr && val != itr->data)
    {
        itr = (val < itr->data)? itr->left : itr->right;
    }

    return itr;
}


/*****************************************
 * DELETION
 *****************************************/

/**
 * Remove u subtree and attach v in u place
 */
static inline void irb_Trasnplant(rbTree *tree, rbNode *u, rbNode *v)
{
    if(u->parent == NULL)
        (*tree) = v;
    else if( u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    if(v)
        v->parent = u->parent;
}


static inline void irb_RemoveFixup(rbTree *tree, rbNode *x, rbNode *xParent)
{
    rbNode *w = NULL;
    enum RB_COLOR xColor = (!x) ? BLACK : x->color; 

    while(x != *tree && xColor== BLACK)
    {
        if(x) xParent = x->parent;

        if(x == xParent->left)
        {
            w = xParent->right;
            if(!w) continue;
            
            if(w && w->color == RED)
            {
                w->color = BLACK;
                xParent->color = RED;
                irb_LeftRotation(tree, xParent);
                w = xParent->right;
            }
            if( (!w->right && !w->left) || ( w->left->color == BLACK && w->right->color == BLACK) )
            {
                w->color = RED;
                x = xParent;
            }
            else 
            {
                if(!w->right || w->right->color == BLACK)
                {
                    if(w->left) w->left->color = BLACK;
                    if(w) w->color = RED;
                    w = xParent->right;
                    irb_RightRotation(tree, w);                   
                }
                w->color = xParent->color;
                xParent->color = BLACK;
                if(w->right) w->right->color = BLACK;
                irb_LeftRotation(tree, xParent);
                x = *tree;
            }
        }
        else
        {
            w = xParent->left;
            if(w->color == RED)
            {
                w->color = BLACK;
                xParent->color = RED;
                w = xParent->left;
                irb_RightRotation(tree, xParent);
            }
            if( (!w->right && !w->left) || ( w->left->color == BLACK && w->right->color == BLACK)  )
            {
                w->color = RED;
                x = xParent;
            }
            else 
            {
                if(w->left->color == BLACK)
                {
                    if(w->right) w->right->color = BLACK;
                    w->color = RED;
                    w= xParent->left;
                    irb_LeftRotation(tree, w);
                }
                w->color = xParent->color;
                xParent->color = BLACK;
                if(w->left) w->left->color = BLACK;
                irb_RightRotation(tree, xParent);
                x = *tree;
            }
        }

        xColor = x->color;
    }

    if(x) x->color = BLACK;
}


/**
 * Delete a node from a tree by reference
 * 
 * @param tree target tree
 * @param node reference to the note to delete
 */
void rb_Delete1(rbTree *tree, rbNode *node)
{
    #ifdef RB_INPUT_CHECK   
    if(!tree)
    {
        DEBUG_PRINT("RB_Delete1: Error, must pass a not null tree pointer!\n");
        return;
    }
    #endif

    if(!node)
    {
        DEBUG_PRINT("RB_Delete1: Error, must pass a not null node pointer!\n");
        return;
    }

    rbNode *x = NULL, *y = NULL, *xParent = NULL;
    enum RB_COLOR yColor = node->color; 

    if(node->left == NULL)
    {
        x = node->right;
        if(!x) xParent = node->parent;
        irb_Trasnplant(tree, node, x);

    }
    else if(node->right == NULL)
    {
        x = node->left;
        if(!x) xParent = node->parent;
        irb_Trasnplant(tree, node, x); 

    }
    else
    {
        y = rb_treeMin(node->right);
        yColor = y->color;
        x = y->right;
        if(y->parent == node)
        {
            if(x) x->parent = y;
        }
        else
        {
            //remove min val in right subtree and reconnect eventual right tree of min val
            irb_Trasnplant(tree, y, y->right);
            y->right = node->right;
            y->right->parent = y;
        }      
        if(!x) xParent = node->parent;
        irb_Trasnplant(tree, node, y);
        y->left = node->left;
        y->left->parent = y;
        y->color = node->color;
    }

    if(yColor == BLACK)
        irb_RemoveFixup(tree, x, xParent);

    DEBUG_PRINT("RB_Delete1: Deleted %d\n", node->data);
    free(node);
}

/**
 * Delete a node from a tree by value
 * 
 * @param tree target tree
 * @param value value of the node to delete
 */
void rb_Delete2(rbTree *tree, int value)
{
    rb_Delete1(tree, rb_Search(*tree, value));
}