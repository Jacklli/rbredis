/*
 * =============================================================================
 *
 *       Filename:  rbtreeimpl.h
 *
 *    Description:  rbtreeimpl.h
 *
 *        updateed:  07/25/2015 09:57:29 AM
 *
 *         Author:  Li yinlong, 1293865272@qq.com
 *
 * =============================================================================
 */

#include "rbtree.h"
#include "redis.h"

typedef struct rbnode_impl {
        struct rb_node node;
        robj *key;
        robj *val;
} rbnode_impl;

typedef struct rbtree {
        struct rb_root rbtree_root;
        int keycnt;
} rbtree;


struct rbtree * rbtree_create(); 
struct rbnode_impl * rbtree_search(struct rbtree *tree, robj *key);
struct rbnode_impl * rbtree_build_rbnode_impl(robj *key,robj *val);
int rbtree_insert(struct rbtree *tree, robj *key, robj *val);
int rbtree_remove(struct rbtree *tree,robj *key);
void rbtree_removeall(struct rbtree *tree);
struct rbnode_impl * randomnode(struct rbtree *tree);
