#include "app.h"
#include "datafile.h"
#include "caut/cauterize_club.h"

#include <assert.h>

static int work(struct peer_set * peer_set, struct record_set * record_set);

int run_with_options(struct options * options) {
  assert(options);

  struct record_set records;

  if (datafile_ok != load_records_from_file(options->datafile, &records)) {
    return 1;
  }

  int result = work(&options->peer_set, &records);

  if (datafile_ok != write_records_to_file(options->datafile, &records)) {
    return 2;
  }

  return result;
}

static int work(struct peer_set * peer_set, struct record_set * record_set) {
  printf("Working...\n");

  return 0;
}
