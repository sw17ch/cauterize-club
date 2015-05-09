#include "app.h"
#include "datafile.h"
#include "caut/cauterize_club.h"

#include <assert.h>

static int work(struct peer_set * peer_set, struct timeline * timeline);

int run_with_options(struct options * options) {
  assert(options);

  struct timeline * timeline;

  if (datafile_ok != load_timeline_from_file(options->datafile, &timeline)) {
    return 1;
  }

  int result = work(&options->peer_set, timeline);

  if (datafile_ok != write_timeline_to_file(options->datafile, timeline)) {
    return 2;
  }

  if (datafile_ok != free_timeline(timeline)) {
    return 3;
  }

  return result;
}

static int work(struct peer_set * peer_set, struct timeline * timeline) {
  (void)peer_set;
  (void)timeline;

  printf("Working...\n");

  return 0;
}
