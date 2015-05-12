#include "app.h"
#include "timeline.h"
#include "hashing.h"
#include "type_utils.h"
#include "caut/cauterize_club.h"

#include <assert.h>
#include <inttypes.h>

static int app_list_entries(const struct timeline * const tl);

static size_t get_user_input(
    void * buffer,
    size_t max_len);

int run_with_options(struct options * options) {
  assert(options);

  enum timeline_status tstat;
  struct timeline * timeline = NULL;

  if (timeline_ok != (tstat = timeline_init_from_file(options->datafile, &timeline))) {
    fprintf(stderr, "Unable to load timeline from %s. (%d)\n", options->datafile, tstat);
    return tstat;
  }

  int result;
  switch (options->mode) {
  case mode_daemon:
  case mode_beginner:
  case mode_joiner:
  case mode_content:
    result = 1;
    break;
  case mode_list:
    result = app_list_entries(timeline);
    break;
  }

  if (timeline_ok != (tstat = timeline_deinit(timeline))) {
    fprintf(stderr, "Unable to uninitialize timeline. (%d)\n", tstat);
    return tstat;
  }

  return 1000 + result;
}

static int app_list_entries(const struct timeline * const tl) {
  char hashstr[HASH_HEX_STR_LEN] = { 0 };
  struct entry_iter * iter = timeline_iterator(tl);

  while(!timeline_iterator_done(iter)) {
    const struct entry_handle * e = timeline_iterator_entry(iter);
    hash_to_str(e->hash, hashstr, sizeof(hashstr));

    printf("%s\n", hashstr);
    memset(hashstr, '=', sizeof(hashstr) - 1);
    printf("%s\n", hashstr);

    printf("%s", "\n");

    timeline_iterator_next(iter);
  }

  timeline_iterator_free(iter);

  return 0;
}

static size_t get_user_input(void * buffer, size_t max_len) {
  return fread(buffer, sizeof(uint8_t), max_len, stdin);
}
