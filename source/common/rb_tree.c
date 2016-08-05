/*******************************************************************************
*Filename:<rb_tree.c>
*
*Brief   :red black tree,uesd to manager logic block,such as data block,empty block
*           ,garbage block .
*
*Owner   :Vito first created at:2016-06-29
*
*Revision:V1.0
*******************************************************************************/
#include <global_def.h>
#include <rb_tree.h>



//×óÐý×ª
static void rb_rotate_left(rb_node *node, rb_root *root)
{
    rb_node *right  = node->right;
    rb_node *parent = rb_parent(node);

    if(node->right = right->left)
        rb_set_parent(right->left, node);
    right->left = node;
    rb_set_parent(right, parent);

    if(parent)
    {
        if(node == parent->left)
            parent->left = right;
        else
            parent->right = right;
    }
    else
        root->rb_node = right;
    rb_set_parent(node, right);
}

//ÓÒÐý×ª
static void rb_rotate_right(rb_node *node, rb_root *root)
{
    rb_node *left = node->left; 
    rb_node *parent = rb_parent(node); 
  
 	if ((node->left = left->right)) 
 		rb_set_parent(left->right, node); 
 	left->right = node; 
  
 	rb_set_parent(left, parent); 
  
 	if (parent) 
 	{ 
 		if (node == parent->right) 
 			parent->right = left; 
 		else 
 			parent->left = left; 
 	} 
 	else 
 		root->rb_node = left; 
 	rb_set_parent(node, left); 
}

void rb_insert_color(rb_node *node, rb_root *root)
{
    rb_node *parent, *gparent;

    while ((parent = rb_parent(node)) && rb_is_red(parent))
    {
        gparent = rb_parent(parent);

        if (parent == gparent->left)
        {
            {
                rb_node *uncle = gparent->right;
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->right == node)
            {
                rb_node *tmp;
                rb_rotate_left(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            rb_rotate_right(gparent, root);
        } else {
            {
                rb_node *uncle = gparent->left;
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->left == node)
            {
                rb_node *tmp;
                rb_rotate_right(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            rb_rotate_left(gparent, root);
        }
    }

    rb_set_black(root->rb_node);
} 

static void rb_erase_color(rb_node *node, rb_node *parent, rb_root *root) 
{ 
	rb_node *other; 

	while ((!node || rb_is_black(node)) && node != root->rb_node) 
	{ 
		if (parent->left == node) 
		{ 
			other = parent->right; 
			if (rb_is_red(other)) 
			{ 
				rb_set_black(other); 
				rb_set_red(parent); 
				rb_rotate_left(parent, root); 
				other = parent->right; 
			} 
			if ((!other->left || rb_is_black(other->left)) && 
			    (!other->right || rb_is_black(other->right))) 
			{ 
				rb_set_red(other); 
				node = parent; 
				parent = rb_parent(node); 
			} 
			else 
			{ 
				if (!other->right || rb_is_black(other->right)) 
				{ 
					rb_set_black(other->left); 
					rb_set_red(other); 
					rb_rotate_right(other, root); 
					other = parent->right; 
				} 
				rb_set_color(other, rb_color(parent)); 
				rb_set_black(parent); 
				rb_set_black(other->right); 
				rb_rotate_left(parent, root); 
				node = root->rb_node; 
				break; 
			} 
		} 
		else 
		{ 
			other = parent->left; 
			if (rb_is_red(other)) 
			{ 
				rb_set_black(other); 
				rb_set_red(parent); 
				rb_rotate_right(parent, root); 
				other = parent->left; 
			} 
			if ((!other->left || rb_is_black(other->left)) && 
			    (!other->right || rb_is_black(other->right))) 
			{ 
				rb_set_red(other); 
				node = parent; 
				parent = rb_parent(node); 
			} 
			else 
			{ 
				if (!other->left || rb_is_black(other->left)) 
				{ 
					rb_set_black(other->right); 
					rb_set_red(other); 
					rb_rotate_left(other, root); 
					other = parent->left; 
				} 
				rb_set_color(other, rb_color(parent)); 
				rb_set_black(parent); 
				rb_set_black(other->left); 
				rb_rotate_right(parent, root); 
				node = root->rb_node; 
				break; 
			} 
		} 
	} 
	if (node) 
		rb_set_black(node); 
} 
  
void rb_erase(rb_node *node, rb_root *root) 
{ 
    rb_node *child, *parent; 
	int color; 

	if (!node->left) 
		child = node->right; 
	else if (!node->right) 
		child = node->left; 
	else 
	{ 
		rb_node *old = node, *left; 

		node = node->right; 
		while ((left = node->left) != NULL) 
			node = left; 

		if (rb_parent(old)) { 
			if (rb_parent(old)->left == old) 
				rb_parent(old)->left = node; 
			else 
				rb_parent(old)->right = node; 
		} else 
			root->rb_node = node; 

		child = node->right; 
		parent = rb_parent(node); 
		color = rb_color(node); 

		if (parent == old) { 
			parent = node; 
		} else { 
			if (child) 
				rb_set_parent(child, parent); 
			parent->left = child; 

			node->right = old->right; 
			rb_set_parent(old->right, node); 
		} 

		node->rb_parent_color = old->rb_parent_color; 
		node->left = old->left; 
		rb_set_parent(old->left, node); 

		goto color; 
	} 

	parent = rb_parent(node); 
	color = rb_color(node); 

	if (child) 
		rb_set_parent(child, parent); 
	if (parent) 
	{ 
		if (parent->left == node) 
			parent->left = child; 
		else 
			parent->right = child; 
	} 
	else 
		root->rb_node = child; 

color: 
	if (color == RB_BLACK) 
		rb_erase_color(child, parent, root); 
} 

#if 0
typedef void (*rb_augment_f)();
//replace rb_augment_f func to void *func //TODO
static void rb_augment_path(rb_node *node, rb_augment_f func, void *data) 
{ 
	rb_node *parent; 

up: 
	func(node, data);  //TODO
	parent = rb_parent(node); 
	if (!parent) 
		return; 

	if (node == parent->left && parent->right) 
		func(parent->right, data); 
	else if (parent->left) 
		func(parent->left, data); 

	node = parent; 
	goto up; 
} 
  
/* 
* after inserting @node into the tree, update the tree to account for 
* both the new entry and any damage done by rebalance 
*/ 
void rb_augment_insert(rb_node *node, rb_augment_f func, void *data) 
{ 
	if (node->left) 
		node = node->left; 
	else if (node->right) 
		node = node->right; 

	rb_augment_path(node, func, data); 
} 
  
/* 
* before removing the node, find the deepest node on the rebalance path 
* that will still be there after @node gets removed 
*/ 
rb_node *rb_augment_erase_begin(rb_node *node) 
{ 
	rb_node *deepest; 

	if (!node->right && !node->left) 
		deepest = rb_parent(node); 
	else if (!node->right) 
		deepest = node->left; 
	else if (!node->left) 
		deepest = node->right; 
	else { 
		deepest = rb_next(node); 
		if (deepest->right) 
			deepest = deepest->right; 
		else if (rb_parent(deepest) != node) 
			deepest = rb_parent(deepest); 
	} 

	return deepest; 
} 
  
/* 
* after removal, update the tree to account for the removed entry 
* and any rebalance damage. 
*/ 
void rb_augment_erase_end(rb_node *node, rb_augment_f func, void *data) 
{ 
	if (node) 
		rb_augment_path(node, func, data); 
} 
#endif

/* 
* This function returns the first node (in sort order) of the tree. 
*/ 
rb_node *rb_first(const rb_root *root) 
{ 
	rb_node	*n; 

	n = root->rb_node; 
	if (!n) 
		return NULL; 
	while (n->left) 
		n = n->left; 
	return n; 
} 
  
rb_node *rb_last(const rb_root *root) 
{ 
	rb_node	*n; 

	n = root->rb_node; 
	if (!n) 
		return NULL; 
	while (n->right) 
		n = n->right; 
	return n; 
} 
  
rb_node *rb_next(const rb_node *node) 
{ 
	rb_node *parent; 

	if (rb_parent(node) == node) 
		return NULL; 

	/* If we have a right-hand child, go down and then left as far 
	   as we can. */ 
	if (node->right) { 
		node = node->right;  
		while (node->left) 
			node=node->left; 
		return (rb_node *)node; 
	} 

	/* No right-hand children.  Everything down and left is 
	   smaller than us, so any 'next' node must be in the general 
	   direction of our parent. Go up the tree; any time the 
	   ancestor is a right-hand child of its parent, keep going 
	   up. First time it's a left-hand child of its parent, said 
	   parent is our 'next' node. */ 
	while ((parent = rb_parent(node)) && node == parent->right) 
		node = parent; 

	return parent; 
} 
  
rb_node *rb_prev(const rb_node *node) 
{ 
	rb_node *parent; 

	if (rb_parent(node) == node) 
		return NULL; 

	/* If we have a left-hand child, go down and then right as far 
	   as we can. */ 
	if (node->left) { 
		node = node->left;  
		while (node->right) 
			node=node->right; 
		return (rb_node *)node; 
	} 

	/* No left-hand children. Go up till we find an ancestor which 
	   is a right-hand child of its parent */ 
	while ((parent = rb_parent(node)) && node == parent->left) 
		node = parent; 

	return parent; 
} 
  
void rb_replace_node(rb_node *victim, rb_node *new, rb_root *root) 
{ 
	rb_node *parent = rb_parent(victim); 

	/* Set the surrounding nodes to point to the replacement */ 
	if (parent) { 
		if (victim == parent->left) 
			parent->left = new; 
		else 
			parent->right = new; 
	} else { 
		root->rb_node = new; 
	} 
	if (victim->left) 
		rb_set_parent(victim->left, new); 
	if (victim->right) 
		rb_set_parent(victim->right, new); 

	/* Copy the pointers/colour from the victim to the replacement */ 
	*new = *victim; 
} 

/*******************************************************************************
*function name:rb_tree_init
*
*description  :init rb tree module reg
*
*parameter    :none
*
*return value :none
*
*revise       :Vito created at:2016-06-29
*******************************************************************************/
void rb_tree_init(void)
{
    //init root node
}
