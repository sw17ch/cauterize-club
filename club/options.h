#ifndef OPTIONS_H
#define OPTIONS_H

#include "caut/cauterize_club.h"

enum mode {
  mode_daemon,
  mode_beginning,
  mode_joiner,
  mode_content,
  mode_list,
};

struct peer_set {
  size_t count;
  struct address * addresses;
};

struct options {
  char * name;
  char * datafile;
  enum mode mode;
  struct peer_set peer_set;
};

enum option_parse_status {
  option_parse_ok,
  option_parse_err_getopt,
  option_parse_err_missing_name,
  option_parse_err_missing_datafile,
};

enum option_parse_status option_parse(int argc, char * argv[], struct options ** options);
void options_free(struct options * options);

#endif /* OPTIONS_H */
