#include "options.h"
#include "list.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

enum option_parse_status option_parse(int argc, char * argv[], struct options * options) {
  assert(options);

  enum option_parse_status status = option_parse_ok;
  int c;

  struct list_node * peer_str_head = NULL;

  opterr = 0;
  while( (c = getopt(argc, argv, "n:p:")) != -1) {
    switch (c) {
    case 'n':
      options->name = optarg;
      break;
    case 'p':
      peer_str_head = list_cons(optarg, peer_str_head);
      break;
    default:
      fprintf(stderr, "Unknown option: %c\n", optopt);
      status = option_parse_err;
      break;
    }
  }

  peer_str_head = list_reverse(peer_str_head);

  if (status == option_parse_ok) {
    size_t peer_count = list_length(peer_str_head);
    printf("Peer Count: %lu\n", peer_count);

    options->peer_set.count = peer_count;
    options->peer_set.addresses = calloc(peer_count, sizeof(options->peer_set.addresses[0]));

    size_t peer_ix = 0;
    struct list_node * cur = peer_str_head;
    while (NULL != cur) {
      struct address * a = &(options->peer_set.addresses[peer_ix]);
      size_t alen = strlen((char*)cur->data);

      strncpy(
          (char *)a->elems,
          cur->data,
          sizeof(a->elems)-1);
      a->_length = alen;

      peer_ix += 1;
      cur = cur->tail;
    }
  }

  list_free(peer_str_head, NULL);
  return status;
}

void options_free(struct options * options) {
  if (NULL != options) {
    if(NULL != options->peer_set.addresses) {
      free(options->peer_set.addresses);
    }
    free(options);
  }
}
