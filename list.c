#include "list.h"

#include <stdlib.h>
#include <stdbool.h>

static bool node_is_nil(struct list_node * node);

void list_free(struct list_node * const node, list_free_cb * cb) {
  assert(node);

  struct list_node * head = node;

  while(head) {
    struct list_node * to_free = head;
    head = head->tail;
    if (cb) {
      cb(to_free);
    }
    free(to_free);
  }
}

size_t list_length(struct list_node * head) {
  assert(head);

  struct list_node * l = head;
  size_t length = 0;

  while(l) {
    length += 1;
    l = l->tail;
  }

  return length;
}

struct list_node * list_cons(void * data, struct list_node * tail) {
  assert(data);

  struct list_node * n = malloc(sizeof(*n));

  n->tail = tail;
  n->data = data;

  return n;
}

/* In-place list reversal. */
struct list_node * list_reverse(struct list_node * head) {
  struct list_node * tail = NULL;
  struct list_node * pos = head;

  while(pos) {
    struct list_node * next_pos = pos->tail;
    pos->tail = tail;
    tail = pos;
    pos = next_pos;
  }

  return tail;
}
