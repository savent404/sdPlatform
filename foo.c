#include <internal/device.h>
#include <internal/dlist.h>

#include <stdint.h>
#include <stdio.h>

struct foo
{
    int val;
    struct dlist dlist;
};

int dlist_test(void)
{
    struct foo a,b,c;
    a.val = 1;
    b.val = 2;
    c.val = 3;
    dlist_init(&a.dlist);
    dlist_init(&b.dlist);
    dlist_init(&c.dlist);

    dlist_add_back(&a.dlist, &b.dlist);
    dlist_add_front(&a.dlist, &c.dlist);

    dlist_t root = &a.dlist;
    dlist_t item = root;

    do {
        struct foo* tmp = _DLIST_GET_NODE(item, dlist, struct foo*);
        printf("intance: 0x%08x Val: %d\n", (size_t)tmp, tmp->val);
        item = item->next;
    } while (root != item);
    return 0;
}

int main(void)
{
    dlist_test();

    return 0;
}