#ifndef _RB_TREE_H_
#define _RB_TREE_H_

#include <global_def.h>

#define     RB_RED         0
#define     RB_BLACK       1
typedef struct st_rb_node
{
    u32                     rb_parent_color;

    struct st_rb_node       *right;
    struct st_rb_node       *left;
}__attribute__((aligned(sizeof(u32))))rb_node;

typedef struct st_rb_root
{
    rb_node *rb_node;
}rb_root;

//get parent node addr
#define rb_parent(r)        ((rb_node *)((r)->rb_parent_color & (~0x03)))
//get node color
#define rb_color(r)         ((r)->rb_parent_color & 0x01)
// test node is red
#define rb_is_red(r)        (!rb_color(r))
//test node is black
#define rb_is_black(r)      rb_color(r)
//set color to red
#define rb_set_red(r)       ((r)->rb_parent_color &= ~1)
//set color to black
#define rb_set_black(r)     ((r)->rb_parent_color |= 1)

#define	rb_entry(ptr, type, member)     container_of(ptr, type, member)
//内联函数，设置节点的父节点

static INLINE void rb_set_parent(rb_node *rb, rb_node *p)
{
    rb->rb_parent_color =(rb->rb_parent_color & 3) | (u32)p;
}

//内联函数，设置节点的color
static INLINE void rb_set_color(rb_node *rb, u8 color)
{
    rb->rb_parent_color =(rb->rb_parent_color & ~1) | color;
}  

#define RB_EMPTY_ROOT(root)    ((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node)    (rb_parent(node) == node)
#define RB_CLEAR_NODE(node)    (rb_set_parent(node, node))


extern void rb_insert_color(rb_node *node, rb_root *root);
extern void rb_erase(rb_node *node, rb_root *root);

/* Find logical next and previous nodes in a tree */
extern rb_node *rb_next(const rb_node *node);
extern rb_node *rb_prev(const rb_node *node);
extern rb_node *rb_first(const rb_root *root);
extern rb_node *rb_last(const rb_root *root);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
extern void rb_replace_node(rb_node *victim, rb_node *new, rb_root *root);

static INLINE void rb_link_node(rb_node * node, rb_node * parent, rb_node ** rb_link)
{
    node->rb_parent_color = (unsigned long )parent;
    node->left = node->right = NULL;

    *rb_link = node;
}

#endif
