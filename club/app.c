#include "app.h"
#include "timeline.h"
#include "hashing.h"
#include "type_utils.h"
#include "caut/cauterize_club.h"

#include <assert.h>

static int work(struct peer_set * peer_set, struct timeline * timeline);
static size_t get_user_input(void * buffer, size_t max_len);

int run_with_options(struct options * options) {
  assert(options);

  enum timeline_status tstat;
  struct timeline * timeline = NULL;

  if (timeline_ok != (tstat = timeline_init_from_file(options->datafile, &timeline))) {
    fprintf(stderr, "Unable to load timeline from %s. (%d)\n", options->datafile, tstat);
    return tstat;
  }

  int result = work(&options->peer_set, timeline);

  if (timeline_ok != (tstat = timeline_deinit(timeline))) {
    fprintf(stderr, "Unable to uninitialize timeline. (%d)\n", tstat);
    return tstat;
  }

  return 1000 + result;
}

static int work(struct peer_set * peer_set, struct timeline * timeline) {
  uint8_t user_buffer[VECTOR_MAX_LEN_name] = { 0 };
  char hstr[HASH_HEX_STR_LEN];

  printf("Enter up to %u bytes of data.\nPress Ctrl+D when finished.\n\n", VECTOR_MAX_LEN_name);
  size_t user_len = get_user_input(user_buffer, sizeof(user_buffer)-1);

  struct entry_handle * new_entry = timeline_new_entry(timeline);

  mk_participant(&new_entry->entry.origin, "i'm a node");
  get_time(&new_entry->entry.timestamp);
  mk_entry_body_beginning(&new_entry->entry.body, (char *)user_buffer);

  hash_buffer(user_buffer, user_len, new_entry->hash);
  hash_to_str(new_entry->hash, hstr, sizeof(hstr));

  printf("================================================================================\n");
  printf("User input: %s\n", user_buffer);
  printf("User len: %lu\n", user_len);
  printf("User hash: %s\n", hstr);
  printf("Time: %llu\n", new_entry->entry.timestamp);

  return 0;
}

static size_t get_user_input(void * buffer, size_t max_len) {
  return fread(buffer, sizeof(uint8_t), max_len, stdin);
}
