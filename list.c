#include "list.h"

#include <stdlib.h>
#include <stdbool.h>

static bool node_is_nil(struct list_node * node);

void list_node_init(struct list_node * node) {
  assert(node);

  node->tail = NULL;
  node->data = NULL;
}

void list_free(struct list_node * node, list_free_cb * cb) {
  assert(node);

  struct list_node * head = NULL;
  struct list_node * tail = node;

  while (tail->tail) {
    head = tail;
    tail = tail->tail;

    if (cb) {
      cb(head->data);
    }
    free(head);
  }
}

size_t list_length(struct list_node * head) {
  assert(head);

  struct list_node * l = head;
  size_t length = 0;

  while (!node_is_nil(l)) {
    length += 1;
    l = l->tail;
  }

  return length;
}

struct list_node * list_cons(void * car, struct list_node * cdr) {
  assert(car);
  assert(cdr);

  struct list_node * n = calloc(1, sizeof(*n));

  n->tail = cdr;
  n->data = car;

  return n;
}

static bool node_is_nil(struct list_node * node) {
  assert(node);

  return !(node->tail);
}

/* In-place list reversal. */
struct list_node * list_reverse(struct list_node * head) {
  struct list_node * current = head;
  struct list_node ** head_tail = &(head->tail);
  struct list_node * new_tail = NULL;

  while(current->tail) {
    struct list_node * next = current->tail;
    current->tail = new_tail;
    new_tail = current;
    current = next;
  }

  *head_tail = current;

  return new_tail;
}
