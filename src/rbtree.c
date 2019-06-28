#include <stdlib.h>
#include <stdio.h>

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

/*****************************************
 * DEBUG
 *****************************************/

#ifdef DEBUG

    void rb_depth_print(rbNode *node, int depth)
    {
        if(node == NULL && depth == 0) printf("- ");
        if(node == NULL) return;
        if(depth == 0)
        {
            printf("(%s%d) ",(node->color == RED)? "R" : "B", node->data);
        }
        else
        {
            rb_depth_print(node->left, depth -1);
            rb_depth_print(node->right, depth -1);
        }       
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
        for(int i = 0; i < rb_height +1; i++)
        {
            for(int j = 0; j < rb_height-i-1; j++)
                printf("     ");
            
            rb_depth_print(root, i);
            printf("\n");
        }
        printf("\n");
    }

#endif

/*****************************************
 * ROTATIONS
 *****************************************/

inline static void rb_LeftRotate(rbNode **root, rbNode* x)
{
    rbNode *y = x->right;

    x->right = y->left;
    if(y->left)
        y->left->parent = x;
    y->parent = x->parent;
    if(x->parent == NULL)
        (*root) = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else 
        x->parent->right = y;
    
    y->left = x;
    x->parent = y;                   
}


inline static void rb_RightRotate(rbNode **root, rbNode* x)
{
    rbNode *y = x->left;

    x->left = y->right;
    if(y->right)
        y->right->parent = x;
    y->parent = x->parent;
    if(x->parent == NULL)
        (*root) = y;
    else if(x == x->parent->right)
        x->parent->right = y;
    else 
        x->parent->left = y;
    
    y->right = x;
    x->parent = y;    
}


/*****************************************
 * INSERTION
 *****************************************/

/**
 * Fix three violations after insertion
 */
inline static void rb_InsertFixup(rbNode **root, rbNode* z)
{
    rbNode *x, *y;
    while(z && z->parent && z->parent->color == RED)
    {
        if(z->parent == z->parent->parent->left)
        {
            y = z->parent->parent->right;
            if(y && y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if(z == z->parent->right)
                {
                    z = z->parent;
                    rb_LeftRotate(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rb_RightRotate(root, z->parent->parent);
            }
            
        }
        else
        {
            y = z->parent->parent->left;
            if(y && y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if(z == z->parent->left)
                    {
                        z = z->parent;
                        rb_RightRotate(root, z);
                    }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rb_LeftRotate(root, z->parent->parent);
            }
        }      
    }
    (*root)->color = BLACK;
}


/**
 * Insert a new node and rebalance tree
 * 
 * @param rb_root root of the tree, can be NULL if tree does not exist
 * @param data data to insert in the tree
 */
void rb_Insert(rbNode **rb_root, int data)
{
    rbNode *new_node = (rbNode*)malloc(sizeof(rbNode));
    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    new_node->color = RED;

    // insert root if tree is empty
    if (*rb_root == NULL)
    {
        new_node->color = BLACK;
        (*rb_root) = new_node;
        return;
    }
    
    //bst insertion
    rbNode *y = NULL;
    rbNode *x = *rb_root;
    while(x != NULL)
    {
        y = x;
        if(data < x->data)
            x = x->left;
        else
            x = x->right;        
    }
    new_node->parent = y;

    if(data < y->data)
        y->left = new_node;
    else
        y->right = new_node;

    rb_InsertFixup(rb_root, new_node);
    printf("Inserted %d\n", data);
}


/*****************************************
 * SEARCH
 *****************************************/

/**
 * Search if a item is in the tree
 * 
 * @param rb_root tree root, can be NULL
 * @param val value to search
 * @return NULL if not found, pointer to tree element if found
 */
rbNode *rb_Search(rbNode *rb_root, int val)
{
    rbNode *itr = rb_root;
    while(itr && val != itr->data)
    {
        if(val < itr->data)
            itr = itr->left;
        else
            itr = itr->right;
    }

    return itr;
}


/*****************************************
 * DELETION
 *****************************************/

/**
 * Remove U and attach V
 */
inline static void rb_Trasnplant(rbNode **rb_root, rbNode *u, rbNode *v)
{
    if(u->parent == NULL)
        (*rb_root) = v;
    else if( u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    if(v)
        v->parent = u->parent;
}


inline static void rb_RemoveFixup(rbNode **rb_root, rbNode *x)
{
    rbNode *w = NULL;

    while(x && x != *rb_root && x->color == BLACK)
    {
        if(x == x->parent->left)
        {
            w = x->parent->right;
            if(w->color == RED)
            {
                w->color = BLACK;
                x->parent->color = RED;
                rb_LeftRotate(rb_root, x->parent);
                w = x->parent->right;
            }
            if(!w->right && !w->left && w->left->color == BLACK && w->right->color == BLACK)
            {
                w->color = RED;
                x = x->parent;
            }
            else 
            {
                if(w->right->color == BLACK)
                {
                    if(w->left) w->left->color = BLACK;
                    w->color = RED;
                    rb_RightRotate(rb_root, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rb_LeftRotate(rb_root, x->parent);
                x = *rb_root;
            }
        }
        else
        {
            w = x->parent->left;
            if(w->color == RED)
            {
                w->color = BLACK;
                x->parent->color = RED;
                rb_RightRotate(rb_root, x->parent);
                w = x->parent->left;
            }
            if(!w->right && !w->left && w->right->color == BLACK && w->left->color == BLACK)
            {
                w->color = RED;
                x = x->parent;
            }
            else 
            {
                if(w->left->color == BLACK)
                {
                    if(w->right) w->right->color = BLACK;
                    w->color = RED;
                    rb_LeftRotate(rb_root, w);
                    w= x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rb_RightRotate(rb_root, x->parent);
                x = *rb_root;
            }
        }
    }

    if(x) x->color = BLACK;
}


inline static rbNode *rb_minimum(rbNode *rb_root)
{
    while(rb_root->left != NULL)
        rb_root = rb_root->left;

    return rb_root;
}

inline static void rb_Remove(rbNode **rb_root, rbNode *z)
{
    rbNode *x = NULL, *y = z;
    enum RB_COLOR yCol = y->color;

    if(z->left == NULL)
    {
        x = z->right;
        rb_Trasnplant(rb_root, z, z->right);
        free(z);
    }
    else if(z->right == NULL)
    {
        x = z->left;
        rb_Trasnplant(rb_root, z, x); 
        free(z);
    }
    else
    {
        y = rb_minimum(z->right);
        yCol = y->color;
        x = y->right;
        if(y->parent == z)
        {
            if(x) x->parent = y;
        }
        else
        {
            rb_Trasnplant(rb_root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        rb_Trasnplant(rb_root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if(yCol == BLACK)
        rb_RemoveFixup(rb_root, x);
    
}



/**
 * Search and delete a node by value
*/
void rb_Delete1(rbNode **rb_root, int data)
{
    rbNode *z = rb_Search(*rb_root, data);
    if(!rb_root && !*rb_root && !z) return;
    rb_Remove(rb_root, z);
}

/**
 * Delete tree node by direct reference
 */
void rbDelete2(rbNode **rb_root, rbNode *node)
{
    if(!rb_root && !*rb_root && !node) return;
    rb_Remove(rb_root, node);
}
