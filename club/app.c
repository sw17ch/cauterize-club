#include "app.h"
#include "timeline.h"
#include "hashing.h"
#include "type_utils.h"
#include "caut/cauterize_club.h"

#include <assert.h>
#include <inttypes.h>
#include <ctype.h>

static int app_list_entries(
    const struct timeline * const tl);

static int app_add_beginning(
    struct timeline * const tl,
    const char * const participant_name,
    const char * const beginning_name);

static size_t get_user_input(
    void * buffer,
    size_t max_len);

/* NOTE: Returns the number of nibbles in byte_buffer. */
static size_t get_user_input_hex(
    char * input_buffer,
    size_t input_buffer_len,
    uint8_t * byte_buffer,
    size_t byte_buffer_len);

static bool ishexchar(char c);

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
  case mode_content:
    result = 1;
    break;
  case mode_joiner:
    {
      hashtype_t first = { 0 };
      hashtype_t second = { 0 };
      char user_input[HASH_HEX_STR_LEN] = {0};
      char hash_str[HASH_HEX_STR_LEN] = {0};

      size_t nibs = get_user_input_hex(user_input, sizeof(user_input),
                                       first, sizeof(first));

      hash_to_str(first, hash_str, sizeof(hash_str));

      printf("you entered: %s (%lu)\n", hash_str, nibs);
    }
    break;
  case mode_beginning:
    {
      printf("Enter up to 512 bytes of data to name this beginning. Ctrl+D when finished.\n\n");
      char user_input[VECTOR_MAX_LEN_name] = { 0 };
      get_user_input(user_input, sizeof(user_input) - 1);
      result = app_add_beginning(timeline, options->name, user_input);
    }
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

static int app_add_beginning(struct timeline * const tl, const char * const participant_name, const char * const beginning_name) {
  struct entry_handle * h = timeline_new_entry(tl);

  memset(h->hash, 0, sizeof(h->hash));
  mk_participant(&h->entry.origin, participant_name);
  get_time(&h->entry.timestamp);
  mk_entry_body_beginning(&h->entry.body, beginning_name);

  return 0;
}

static size_t get_user_input(void * buffer, size_t max_len) {
  return fread(buffer, sizeof(uint8_t), max_len, stdin);
}

static size_t get_user_input_hex(char * input_buffer, size_t input_buffer_len, uint8_t * byte_buffer, size_t byte_buffer_len) {
  memset(input_buffer, 0, input_buffer_len);
  memset(byte_buffer, 0, byte_buffer_len);

  ssize_t s_input_len = getline(&input_buffer, &input_buffer_len, stdin);
  size_t input_len = 0;

  if (s_input_len < 0) {
    return 0;
  } else if (s_input_len >= 1) {
    // Throw away the \n
    input_buffer[s_input_len - 1] = 0;
    input_len = s_input_len - 1;
  } else {
    input_len = 0;
  }

  for (size_t i = 0; i < input_len - 1; i++) {
    input_buffer[i] = tolower(input_buffer[i]);
    if (!ishexchar(input_buffer[i])) {
      return 0;
    }
  }

  for (size_t i = 0; (i < input_len) && (i / 2 < byte_buffer_len); i += 2) {
    uint8_t byte = 0;
    sscanf(&input_buffer[i], "%02hhx", &byte);

    if (input_len - i == 1) {
      byte_buffer[i/2] = byte << 4;
    } else {
      byte_buffer[i/2] = byte;
    }
  }

  return input_len;
}

static bool ishexchar(char c) {
  return (('A' <= c && c <= 'F') ||
          ('a' <= c && c <= 'f') ||
          ('0' <= c && c <= '9'));
}
