#ifndef OPTIONS_H
#define OPTIONS_H

#include "caut/cauterize_club.h"

struct peer_set {
  size_t count;
  struct peer * peers;
};

struct options {
  struct peer_set peers;
};

enum option_parse_status {
  option_parse_ok,
  option_parse_err,
};

enum option_parse_status option_parse(int argc, char * argv[], struct options ** options);
void options_free(struct options * options);

#endif /* OPTIONS_H */
