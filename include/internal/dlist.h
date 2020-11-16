#pragma once

#include <internal/toolchain.h>
#include <type.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct dlist
{
    struct dlist* next;
    struct dlist* prev;
} * dlist_t;

static inline void dlist_init(dlist_t list)
{
    list->next = list;
    list->prev = list;
}

static inline void dlist_add(dlist_t _prev, dlist_t _next, dlist_t _new)
{
    _new->next = _next;
    _new->prev = _prev;
    _next->prev = _new;
    __IO(_prev->next) = _new;
}

static inline void dlist_del(dlist_t _prev, dlist_t _next, dlist_t _node)
{
    __IO(_prev->next) = _next;
    _next->prev = _prev;

    dlist_init(_node);
}

static inline void dlist_add_back(dlist_t _node, dlist_t _new)
{
    dlist_add(_node, _node->next, _new);
}

static inline void dlist_add_front(dlist_t _node, dlist_t _new)
{
    dlist_add(_node->prev, _node, _new);
}

static inline void dlist_remove(dlist_t _node)
{
    dlist_del(_node->prev, _node->next, _node);
}

static inline dlist_t dlist_next(dlist_t _node) { return _node->next; }

static inline dlist_t dlist_prev(dlist_t _node) { return _node->prev; }

static inline int dlist_isalone(dlist_t _node)
{
    return _node->next == _node;
}

static inline bool dlist_has_item(dlist_t _head, dlist_t _node)
{
    dlist_t item = _head;
    do {
        item = _head->next;
        if (item == _node)
            return true;
    } while (item != _head);
    return false;
}

#define _DLIST_GET_NODE(instance, member_name, node_ptr_type)                  \
    ({                                                                         \
        node_ptr_type __s = NULL;                                              \
        void* __offset = &__s->member_name;                                    \
        char* __instance = (char*)(instance);                                  \
        (node_ptr_type)(__instance - (size_t)__offset);                        \
    })

#define DLIST_GET_NODE(instance, node_ptr_type)                                \
    _DLIST_GET_NODE(instance, dlist, node_ptr_type)

#ifdef __cplusplus
}
#endif