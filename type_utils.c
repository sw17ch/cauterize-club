#include "type_utils.h"

#include <sys/time.h>

static void mk_name(struct name * n, char * name_str);

void mk_participant(struct participant * p, char * name_str) {
  mk_name(&p->name, name_str);
}

static void mk_name(struct name * n, char * name_str) {
  strncpy((char *)n->elems, name_str, sizeof(n->elems)-1);
  n->_length = strnlen((char *)n->elems, sizeof(n->elems));
}

void get_time( uint64_t * ns_dst) {
  struct timeval tv = { 0 };

  if (0 == gettimeofday(&tv, NULL)) {
    *ns_dst = (1000000000 * tv.tv_sec) + (1000 * tv.tv_usec);
  } else {
    *ns_dst = 0;
  }
}

void mk_entry_body_beginning(struct entry_body * body, char * name_str) {
  body->_tag = entry_body_tag_beginning;
  mk_name(&body->beginning.name, name_str);
}

