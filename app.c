#include "app.h"
#include "datafile.h"
#include "hashing.h"
#include "type_utils.h"
#include "caut/cauterize_club.h"

#include <assert.h>

static int work(struct peer_set * peer_set, struct timeline * timeline);
static size_t get_user_input(void * buffer, size_t max_len);

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
  hashtype_t hash;
  char hstr[HASH_HEX_STR_LEN];
  uint8_t user_buffer[VECTOR_MAX_LEN_name] = { 0 };

  printf("Enter up to %u bytes of data.\nPress Ctrl+D when finished.\n\n", VECTOR_MAX_LEN_name);
  size_t user_len = get_user_input(user_buffer, sizeof(user_buffer)-1);

  struct entry new_entry;
  mk_participant(&new_entry.origin, "i'm a node");
  get_time(&new_entry.timestamp);

  mk_entry_body_beginning(&new_entry.body, (char *)user_buffer);

  hash_buffer(user_buffer, user_len, hash);
  hash_to_str(hash, hstr, sizeof(hstr));

  printf("================================================================================\n");
  printf("User input: %s\n", user_buffer);
  printf("User len: %lu\n", user_len);
  printf("User hash: %s\n", hstr);
  printf("Time: %llu\n", new_entry.timestamp);

  return 0;
}

static size_t get_user_input(void * buffer, size_t max_len) {
  return fread(buffer, sizeof(uint8_t), max_len, stdin);
}
