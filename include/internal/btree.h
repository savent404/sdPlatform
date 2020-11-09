#pragma once

#include <internal/dlist.h>



typedef struct btree
{
    struct dlist dlist;
    struct btree* child;
    struct btree* parent;
} * btree_t;

static inline void
btree_init(btree_t tree)
{
    dlist_init(&tree->dlist);
    tree->child = NULL;
    tree->parent = NULL;
}

static inline void
btree_add_child(btree_t _node, btree_t _new)
{
    // make sure '_new' is alone
    dlist_init(&_new->dlist);
    _new->parent = _node;

    if (_node->child) {
        btree_t brother = _node->child;
        dlist_add_front(&brother->dlist, &_new->dlist);
    } else {
        _node->child = _new;
    }
}

static inline void
btree_del_child(btree_t _parent, btree_t _node)
{
    if (!_parent || !_parent->child ||
        !dlist_has_item(&_parent->child->dlist, &_node->dlist))
        return;
    dlist_t loop = &_parent->child->dlist;
    dlist_t target = &_node->dlist;
    if (dlist_isalone(loop)) {
        _parent->child = NULL;
        _node->parent = NULL;
    } else if (target == loop) {
        _parent->child = DLIST_GET_NODE(_node->dlist.next, btree_t);
        dlist_remove(target);
    } else {
        dlist_remove(target);
    }
}

static inline void
btree_remove(btree_t _node)
{
    if (_node->parent) {
        btree_del_child(_node->parent, _node);
    }
}

static inline btree_t
btree_child(btree_t _node)
{
    return _node->child;
}

static inline btree_t
btree_parent(btree_t _node)
{
    return _node->parent;
}

static inline btree_t
btree_next(btree_t _node)
{
    dlist_t next = dlist_next(&_node->dlist);

    return DLIST_GET_NODE(next, btree_t);
}

#define _BTREE_GET_NODE _DLIST_GET_NODE

#define BTREE_GET_NODE(instance, node_ptr_type)                                \
    _BTREE_GET_NODE(instance, btree, node_ptr_type)


