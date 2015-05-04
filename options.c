#include "options.h"
#include "list.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

enum option_parse_status option_parse(int argc, char * argv[], struct options ** options) {
  assert(options);

  enum option_parse_status status = option_parse_ok;
  int c;

  struct list_node * peer_str_head = calloc(1, sizeof(*peer_str_head));
  list_node_init(peer_str_head);

  *options = calloc(1, sizeof(*options));
  opterr = 0;

  while( (c = getopt(argc, argv, "f:p:")) != -1) {
    switch (c) {
    case 'f':
      fprintf(stderr, "FILE %s\n", optarg);
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

  if (status == option_parse_ok) {
    size_t peer_count = list_length(peer_str_head);

    (*options)->peers.count = peer_count;
    (*options)->peers.peers = calloc(peer_count, sizeof((*options)->peers.peers[0]));

    size_t peer_ix = 0;
    struct list_node * cur = peer_str_head;
    while (NULL != cur->tail){
      struct address * a = &(*options)->peers.peers[peer_ix].address;
      size_t alen = strlen((char*)cur->data);

      strncpy(
          (char*)&a->elems,
          (char*)cur->data,
          sizeof(a->elems));
      a->_length = alen;

      cur = cur->tail;
    }
  }

  peer_str_head = list_reverse(peer_str_head);
  list_free(peer_str_head, NULL);
  return status;
}

void options_free(struct options * options) {
  if (NULL != options) {
    if(NULL != options->peers.peers) {
      free(options->peers.peers);
    }
    free(options);
  }
}
