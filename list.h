#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <assert.h>

struct list_node {
  struct list_node * tail;
  void * data;
};

typedef void (list_free_cb)(void * data);

void list_node_init(struct list_node * node);
void list_free(struct list_node * node, list_free_cb * cb);
size_t list_length(struct list_node * node);
struct list_node * list_cons(void * car, struct list_node * cdr);
struct list_node * list_reverse(struct list_node * head);

#endif /* LIST_H */
