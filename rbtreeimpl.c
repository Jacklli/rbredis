/*
 * =============================================================================
 *
 *       Filename:  rbtreeimpl.c
 *
 *    Description:  rbtree implemens for Red-Black redis
 *
 *        Created:  07/25/2015 09:57:29 AM
 *
 *         Author:  Li yinlong, 1293865272@qq.com
 *
 * =============================================================================
 */

#include "rbtree.h"
#include "sds.h"
#include "zmalloc.h"
#include "dict.h"
#include "rbtreeimpl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct rbtree * rbtree_create() {
	struct rbtree * tree = NULL;
	tree = (struct rbtree *)zmalloc(sizeof(* tree));
	tree->rbtree_root = RB_ROOT;
	tree->keycnt = 0;
	return tree;
}

struct rbnode_impl * rbtree_search(struct rbtree *tree, robj *key) {
	int result;
  	struct rb_node *node = (&(tree->rbtree_root))->rb_node;
  	while (node) {
  		struct rbnode_impl *data = container_of(node, struct rbnode_impl, node);
		result = strcmp(key->ptr,data->key->ptr);
		if (result < 0)
  			node = node->rb_left;
		else if (result > 0)
  			node = node->rb_right;
		else
  			return data;
	}
	return NULL;
}

struct rbnode_impl * rbtree_build_rbnode_impl(robj *key,robj *val) {
        struct rbnode_impl *nd_impl = NULL;
        nd_impl = (struct rbnode_impl *)zmalloc(sizeof(*nd_impl));
        nd_impl->key = key;
        nd_impl->val = val;
        return nd_impl;
}

int rbtree_insert(struct rbtree *tree, robj *key, robj *val) {
	struct rbnode_impl *node;
	if(node = rbtree_search(tree,key)) {	//if the key exists free the old and replace it with the new val
		decrRefCount(node->val);
		node->val = val;
		return DICT_OK;
	} else {
		node = rbtree_build_rbnode_impl(key,val);
		struct rb_node **new = &((&(tree->rbtree_root))->rb_node), *parent = NULL;

		/* Figure out where to put new node */
		while (*new) {
			struct rbnode_impl *this = container_of(*new, struct rbnode_impl, node);
			int result = strcmp(node->key->ptr, this->key->ptr);
			parent = *new;
			if (result < 0)
				new = &((*new)->rb_left);
			else if (result > 0)
				new = &((*new)->rb_right);
			else
				return DICT_ERR;
		}

		/* Add new node and rebalance tree. */
		rb_link_node(&node->node, parent, new);
		rb_insert_color(&node->node, &(tree->rbtree_root));
		tree->keycnt++;
		return DICT_OK;
	}
	return DICT_ERR;
}

int rbtree_remove(struct rbtree *tree,robj *key) {
	struct rbnode_impl *node = NULL;
	node = rbtree_search(tree,key); 
	if (node) {
		if(node->key) {
			if(node->key->ptr) {
				decrRefCount(node->key);
				node->key = NULL;
			}
		}
		if(node->val) {
			if(node->val->ptr) {
				decrRefCount(node->val);
                        	node->val = NULL;
			}
		}

		rb_erase(&node->node, &(tree->rbtree_root));	
		zfree(node);
		tree->keycnt--;
		node = NULL;
	}
	return DICT_OK;
}

void rbtree_removeall(struct rbtree *tree) {
	struct rb_node *node = NULL;
	struct rbnode_impl *nd;
	while(tree->keycnt) {
		node = rb_last(&(tree->rbtree_root));
		nd = rb_entry(node, struct rbnode_impl, node);
		rbtree_remove(tree,nd->key);
	}
}

struct rbnode_impl * randomnode(struct rbtree *tree) {
	int i = tree->keycnt;
	int j = 0;
	int k = 0;
	struct rb_node *node = NULL;
        struct rbnode_impl *nd;

	srand(time(0));
	while(!(j = rand()%(i+1)));
	for (node = rb_first(&(tree->rbtree_root));k < j && node;node = rb_next(node)) {
		nd = rb_entry(node, struct rbnode_impl, node);
		k++;
	}
	
	return nd;
}
