#ifndef KLIB_LIST_H_
#define KLIB_LIST_H_

#include "kernel/kmm.h"
#include <stdint.h>
#include "kernel/kernel.h"


typedef struct list_head {
    struct list_head *next, *prev;
} list_head_t;

// name: list_head name
#define LIST_HEAD_INIT(name) {&(name), &(name)}
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
    (ptr)->next = (ptr);(ptr)->prev = (ptr); \
} while(0)


static inline void _list_add(struct list_head *new, struct list_head *left, struct list_head *right) {
    new -> next = right;
    new -> prev = left;
    left -> next = new;
    right -> prev = new;
}

static inline void list_add(struct list_head *new, struct list_head *head) {
    _list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head) {
    _list_add(new, head->prev, head);
}

static inline void _list_del(struct list_head *prev, struct list_head *next) {
    prev->next = next;
    next->prev = prev;
}

// remove the node from list
static inline void list_del(struct list_head *entry) {
    _list_del(entry->prev, entry->next);
    entry->next = NULL;
    entry->prev = NULL;
}

static inline void list_replace(struct list_head *old, struct list_head *new) {
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

static inline void list_move_to_head(struct list_head *node, struct list_head *dst_list) {
    list_del(node);
    list_add(node, dst_list);
}

static inline void list_move_to_tail(struct list_head *node, struct list_head * dst_list) {
    list_del(node);
    list_add_tail(node, dst_list);
}

static inline void list_swap(struct list_head *entry1, struct list_head *entry2) {
    struct list_head *pos = entry2->prev;
	list_del(entry2);
	list_replace(entry1, entry2);
	if (pos == entry1)
		pos = entry2;
	list_add(entry1, pos);
}

static inline int list_is_first(struct list_head *entry, struct list_head *head) {
    return head->next == entry;
}


static inline int list_is_last(struct list_head *entry, struct list_head *head) {
    return head->prev == entry;
}

#endif // KLIB_LIST_H_