#include "options.h"
#include "list.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <getopt.h>

static void print_usage(char * cmd) {
    fprintf(stderr,
        "Usage:\n\n\t%s -n NODE_NAME -d DATA_FILE -p PEER_ADDR [-p ADDITIONAL_PEER]\n\n",
        cmd);
}

enum option_parse_status option_parse(int argc, char * argv[], struct options ** opts) {
  assert(opts);

  *opts = calloc(1, sizeof(**opts));
  struct options * options = *opts;

  memset(options, 0, sizeof(*options));

  enum option_parse_status status = option_parse_ok;
  int c;

  struct list_node * peer_str_head = NULL;

  opterr = 0;
  while( (c = getopt(argc, argv, "n:p:d:")) != -1) {
    switch (c) {
    case 'n':
      options->name = optarg;
      break;
    case 'd':
      options->datafile = optarg;
      break;
    case 'p':
      peer_str_head = list_prepend(optarg, peer_str_head);
      break;
    default:
      fprintf(stderr, "Unknown option: %c\n", optopt);
      status = option_parse_err_getopt;
      break;
    }
  }

  peer_str_head = list_reverse(peer_str_head);

  if (option_parse_ok != status) {
    print_usage(argv[0]);
  } else if (NULL == options->name) {
    fprintf(stderr, "No node name specified (option -n).\n");
    status = option_parse_err_missing_name;
  } else if (NULL == options->datafile) {
    fprintf(stderr, "No datafile path specified (option -p).\n");
    status = option_parse_err_missing_datafile;
  } else {
    size_t peer_count = list_length(peer_str_head);

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
