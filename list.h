#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <assert.h>

struct list_node {
  struct list_node * tail;
  void * data;
};

typedef void (list_free_cb)(void * data);

void list_free(struct list_node * const node, list_free_cb * const cb);
size_t list_length(struct list_node * const node);
struct list_node * list_prepend(void * const data, struct list_node * const tail);
struct list_node * list_reverse(struct list_node * const head);

#endif /* LIST_H */
