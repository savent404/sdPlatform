#include <internal/dlist.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void
print_help(void)
{
    printf("Usage dlist-test cmd\n");
}

static bool
test_init(void)
{
    struct dlist root;
    bool res = true;
    dlist_init(&root);

    res &= root.prev == &root;
    res &= root.next == &root;

    return res;
}

static bool
test_add(void)
{
    struct dlist first;
    struct dlist second;
    bool res = true;

    dlist_init(&first);
    // dlist_init(&second);
    dlist_add_front(&first, &second);

    res &= first.next == &second;
    res &= first.prev == &second;
    res &= second.next == &first;
    res &= second.prev == &first;

    dlist_init(&first);
    // dlist_init(&second);
    dlist_add_back(&first, &second);

    res &= first.next == &second;
    res &= first.prev == &second;
    res &= second.next == &first;
    res &= second.prev == &first;

    return res;
}

static bool
test_remove(void)
{
    struct dlist first;
    struct dlist third;
    struct dlist second;
    bool res = true;

    dlist_init(&first);

    dlist_add_front(&first, &second);
    dlist_add_front(&first, &third);
    dlist_remove(&third);

    res &= first.next == &second;
    res &= first.prev == &second;
    res &= second.next == &first;
    res &= second.prev == &first;

    return res;
}

static bool
test_isalone(void)
{
    struct dlist root, second;
    bool res = true;
    dlist_init(&root);
    res &= dlist_isalone(&root);
    dlist_add_front(&root, &second);
    res &= !dlist_isalone(&root);
    return res;
}

struct foo
{
    int val;
    struct dlist dlist;
};
static bool
test_get_instance(void)
{
    struct foo tmp;
    dlist_t list = &tmp.dlist;

    return &tmp == DLIST_GET_NODE(list, struct foo*);
}

int
main(int argc, char* argv[])
{
    if (argc != 1) {
        printf("parameter error\n");
        return -1;
    }

    bool res = true;

    res &= test_init();
    res &= test_add();
    res &= test_remove();
    res &= test_get_instance();
    return res ? 0 : -1;
}
