#ifndef DATAFILE_H
#define DATAFILE_H

#include "caut/cauterize_club.h"

enum datafile_status {
  datafile_ok,
  datafile_err,
};

struct timeline;

enum datafile_status load_timeline_from_file(
    const char * const path,
    struct timeline ** const tl);

enum datafile_status write_timeline_to_file(
    const char * const path,
    const struct timeline * const tl);

enum datafile_status free_timeline(struct timeline * tl);

#endif /* DATAFILE_H */
