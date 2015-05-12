#include "timeline_internal.h"
#include "hashing.h"

#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

#define S enum timeline_status
#define T struct timeline

#define DEFAULT_REGION_SIZE (8)

void timeline_init(T ** tl, const char * const path);
static struct timeline_region * alloc_region(size_t size);
static void free_regions(struct timeline_region * r);

static bool file_readable(const char * const path, int * const eno);
static S read_and_validate_header(FILE * f);

static bool file_writeable(const char * const path, int * const eno);
static S write_header(FILE * f);
static S write_entry(FILE * f, struct message_cauterize_club * m);

static S save_timeline_to_disk(const T * const tl);

S timeline_init_from_file(const char * const path, T ** const tl) {
  timeline_init(tl, path);
  T * timeline = *tl;

  int eno = 0;
  if (!file_readable(path, &eno)) {
    if (ENOENT != eno) {
      fprintf(stderr, "Unable to read file `%s`. Reason given: %s\n", path, strerror(eno));
      return timeline_err_path_not_readable;
    }
  } else {
    FILE * datafile = fopen(path, "r");
    {
      S stat;

      if (timeline_ok != (stat = read_and_validate_header(datafile))) {
        fprintf(stderr, "Unable to validate datafile header: %d\n", stat);
        return stat;
      }

      // Header validated. Read in timeline.
      // TODO: read entries
    }
    fclose(datafile);
  }

  return timeline_ok;
}

S timeline_deinit(T * const tl) {
  if (NULL != tl) {
    S dstat;
    if (timeline_ok != (dstat = save_timeline_to_disk(tl))) {
      return dstat;
    } else {
      if (NULL != tl->transcode_buffer) {
        free(tl->transcode_buffer);
      }
      free(tl);
    }
  }

  return timeline_ok;
}

void timeline_init(T ** tl, const char * const path) {
  T * timeline = *tl = calloc(1, sizeof(*timeline));
  memset(timeline, 0, sizeof(*timeline));
  timeline->regions = NULL;
  timeline->transcode_buffer = malloc(MESSAGE_MAX_SIZE_cauterize_club);
  timeline->file_path = path;
}

static struct timeline_region * alloc_region(size_t size) {
  struct timeline_region * r = calloc(1, sizeof(*r));

  r->elems = calloc(size, sizeof(*r->elems));
  r->used = 0;
  r->count = size;
  r->next = NULL;

  return r;
}

void free_regions(struct timeline_region * regions) {
  struct timeline_region * r = regions;

  while(r) {
    struct timeline_region * to_free = r;
    free(r->elems);
    r = r->next;
    free(to_free);
  }
}

static bool file_readable(const char * const path, int * const eno) {
  int a = access(path, R_OK);
  if (NULL != eno) { *eno = errno; }
  return (0 == a);
}

static S read_and_validate_header(FILE * f) {
  hashtype_t schema_hash = { 0 };
  size_t read_items = fread(schema_hash, sizeof(schema_hash), 1, f);

  if (1 != read_items) {
    return timeline_err_header_not_enough_data;
  } else {
    if (0 != memcmp(schema_hash, SCHEMA_HASH_cauterize_club, sizeof(schema_hash))) {
      char hash_exp[HASH_HEX_STR_LEN] = { 0 };
      char hash_act[HASH_HEX_STR_LEN] = { 0 };

      hash_to_str(SCHEMA_HASH_cauterize_club, hash_exp, sizeof(hash_exp));
      hash_to_str(schema_hash, hash_act, sizeof(hash_act));

      fprintf(stderr, "Hash mismatch:\n"
                      "  Expected: %s\n"
                      "  Actual:   %s\n", hash_exp, hash_act);

      return timeline_err_header_mismatch;
    } else {
      return timeline_ok;
    }
  }
}

S save_timeline_to_disk(const T * const tl) {
  assert(tl);
  assert(tl->file_path);

  // TODO: Using rename like this only works if /tmp and path are on the same
  // file system. Try and make the temp file beside parent directory of `path`.

  char temp_path[] = "/tmp/cauterize-club.tmp.XXXXXXXXXXXXXXXX";
  int fd = mkstemp(temp_path);
  FILE * temp = fdopen(fd, "w");
  S wstat;

  int eno = 0;
  if (!file_writeable(tl->file_path, &eno)) {
    if (ENOENT != eno) {
      fprintf(stderr, "Unable to write file `%s`. Reason given: %s\n", tl->file_path, strerror(eno));
      return timeline_err_path_not_writeable;
    }
  }

  if (timeline_ok != (wstat = write_header(temp))) {
    fputs("Unable to write header.\n", stderr);
    return wstat;
  }

  // Header written. Write timeline.
#if 0
  for (size_t i = 0; i < tl->count; i++) {
    if (write_status_ok != (wstat = write_entry(temp, &tl->entries[i]))) {
      fprintf(stderr, "Unable to write entry: %d\n", wstat);
    }
  }
#endif

  fclose(temp);

  if (0 != rename(temp_path, tl->file_path)) {
    int e = errno;
    fprintf(stderr, "Unable rename temp timeline file: %s\n", strerror(e));
    return timeline_err_unable_to_rename_temp;
  } else {
    return timeline_ok;
  }
}

static bool file_writeable(const char * const path, int * const eno) {
  int a = access(path, W_OK);
  if (NULL != eno) { *eno = errno; }
  return (0 == a);
}

static S write_header(FILE * f) {
  size_t written =
    fwrite(SCHEMA_HASH_cauterize_club, sizeof(SCHEMA_HASH_cauterize_club), 1, f);

  if (1 != written) {
    return timeline_err_unable_to_write_header;
  } else {
    return timeline_ok;
  }
}

static S write_entry(FILE * f, struct message_cauterize_club * m) {
  enum caut_status cstat = caut_status_ok;

  // TODO: This can be the max size of the entry instead of message.
  void * enc_buffer = calloc(MAX_SIZE_cauterize_club, sizeof(uint8_t));
  struct caut_encode_iter enc_iter;

  struct message_cauterize_club _m = {
    ._type = type_index_cauterize_club_u8,
    ._data = {
      .msg_u8 = 'a',
    },
  };

  caut_encode_iter_init(&enc_iter, enc_buffer, MAX_SIZE_cauterize_club);
  if (caut_status_ok != (cstat = encode_message_cauterize_club(&enc_iter, &_m))) {
    return timeline_err_unable_to_encode_entry;
  }

  size_t len = caut_encode_iter_used(&enc_iter);

  if (len != fwrite(enc_buffer, sizeof(uint8_t), len, f)) {
    return timeline_err_unable_to_write_encoded_entry;
  } else {
    return timeline_ok;
  }
}

#undef S
#undef T
