#ifndef DATAFILE_H
#define DATAFILE_H

#include "caut/cauterize_club.h"

enum datafile_status {
  datafile_ok,
  datafile_err,
};

struct record_set {
  size_t count;
  struct message_cauterize_club * records;
};

enum datafile_status load_records_from_file(
    const char * const path,
    struct record_set * const set);

enum datafile_status write_records_to_file(
    const char * const path,
    const struct record_set * const set);

#endif /* DATAFILE_H */
