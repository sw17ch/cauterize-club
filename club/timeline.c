#include "caut/cauterize_club.h"

#include "timeline_internal.h"
#include "hashing.h"

#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

#define S enum timeline_status
#define T struct timeline

#define DEFAULT_REGION_SIZE (8)

static void timeline_init(T ** tl, const char * const path);
static S timeline_save_to_disk(const T * const tl);

static bool file_readable(const char * const path, int * const eno);
static S read_and_validate_header(FILE * f);
static S read_entry_buffer(FILE * f, void * buff, size_t len, file_len_hdr_t * lh);

static bool file_writeable(const char * const path, int * const eno);
static S write_header(FILE * f);
static S write_entry(FILE * f, struct entry_handle * m, uint8_t * enc_buffer, size_t buff_len);

static struct timeline_region * alloc_region(size_t size);
static void free_regions(struct timeline_region * r);

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

      // Header validated. Read in timeline one entry at a time.
      while(0 == feof(datafile)) {
        file_len_hdr_t len_hdr = 0;
        stat = read_entry_buffer(datafile,
                                 timeline->transcode_buffer,
                                 timeline->transcode_buffer_len,
                                 &len_hdr);
        if (stat == timeline_err_eof) {
          break;
        } else if (stat != timeline_ok) {
          return stat;
        } else {
          hashtype_t hash = { 0 };
          struct caut_decode_iter di;
          struct entry_handle * eh = timeline_new_entry(timeline);
          uint8_t * hash_start = &timeline->transcode_buffer[0];
          uint8_t * data_start = &timeline->transcode_buffer[sizeof(eh->hash)];
          size_t data_len = len_hdr - sizeof(eh->hash);

          hash_buffer(data_start, data_len, hash);

          if (0 != memcmp(hash, hash_start, sizeof(eh->hash))) {
            char exp[HASH_HEX_STR_LEN] = {0};
            char act[HASH_HEX_STR_LEN] = {0};

            hash_to_str(hash_start, exp, sizeof(exp));
            hash_to_str(hash, act, sizeof(exp));

            fprintf(stderr,
                "Unexpected entry hash.\n"
                "\tExpected: %s\n"
                "\tActual:   %s\n",
                exp, act);

            return timeline_err_entry_hash_mismatch;
          }

          memcpy(eh->hash, hash_start, sizeof(eh->hash));

          caut_decode_iter_init(
              &di, data_start, data_len);

          if (caut_status_ok != decode_entry(&di, &eh->entry)) {
            return timeline_err_unable_to_decode_entry;
          }
          assert(0 == caut_decode_iter_remaining(&di));
        }
      }
    }
    fclose(datafile);
  }

  return timeline_ok;
}

S timeline_deinit(T * const tl) {
  if (NULL != tl) {
    S dstat;
    if (timeline_ok != (dstat = timeline_save_to_disk(tl))) {
      return dstat;
    } else {
      if (NULL != tl->transcode_buffer) {
        free(tl->transcode_buffer);
      }
      if (NULL != tl->regions) {
        free_regions(tl->regions);
      }
      free(tl);
    }
  }

  return timeline_ok;
}

struct entry_handle * timeline_new_entry(T * const tl) {
  assert(tl);

  struct timeline_region * current_region = tl->regions;

  while(true) {
    if (current_region->used >= current_region->count) {
      if (NULL == current_region->next) {
        current_region->next = alloc_region(DEFAULT_REGION_SIZE);
      }
      current_region = current_region->next;
    } else {
      struct entry_handle * e = &current_region->elems[current_region->used];
      current_region->used += 1;
      return e;
    }
  }
}

const struct entry_handle * timeline_last_entry(const T * const tl) {
  assert(tl);

  struct timeline_region * current_region = tl->regions;

  while(current_region) {
    if (current_region->used >= current_region->count) {
      current_region = current_region->next;
    } else {
      if (0 < current_region->used) {
        return &current_region->elems[current_region->used - 1];
      }
    }
  }

  return NULL;
}

struct entry_iter * timeline_iterator(const struct timeline * const tl) {
  struct entry_iter * iter = calloc(1, sizeof(*iter));
  iter->tl = tl;

  timeline_iterator_reset(iter);

  return iter;
}

void timeline_iterator_reset(struct entry_iter * iter) {
  iter->pos = 0;

  if (iter->tl->regions->used > 0) {
    iter->region = iter->tl->regions;
    iter->entry = &iter->tl->regions->elems[0];
  } else {
    iter->region = NULL;
    iter->entry = NULL;
  }
}

void timeline_iterator_next(struct entry_iter * iter) {
  iter->entry = NULL;
  iter->pos += 1;

  while (iter->region) {
    if (iter->pos >= iter->region->used) {
      iter->pos = 0;
      iter->region = iter->region->next;
    } else {
      iter->entry = &iter->region->elems[iter->pos];
      break;
    }
  }
}

bool timeline_iterator_done(struct entry_iter * iter) {
  return (NULL == iter->entry);
}


const struct entry_handle * timeline_iterator_entry(const struct entry_iter * const iter) {
  return iter->entry;
}

void timeline_iterator_free(struct entry_iter * iter) {
  if (NULL != iter) {
    free(iter);
  }
}

static void timeline_init(T ** tl, const char * const path) {
  T * timeline = *tl = calloc(1, sizeof(*timeline));
  memset(timeline, 0, sizeof(*timeline));
  timeline->transcode_buffer = malloc(MESSAGE_MAX_SIZE_cauterize_club);
  timeline->transcode_buffer_len = MESSAGE_MAX_SIZE_cauterize_club;
  timeline->file_path = path;
  timeline->regions = alloc_region(DEFAULT_REGION_SIZE);
}

static struct timeline_region * alloc_region(size_t size) {
  struct timeline_region * r = calloc(1, sizeof(*r));

  r->elems = calloc(size, sizeof(*r->elems));
  r->used = 0;
  r->count = size;
  r->next = NULL;

  return r;
}

static void free_regions(struct timeline_region * regions) {
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

static S read_entry_buffer(FILE * f, void * buff, size_t len, file_len_hdr_t * lh) {
  size_t read_items;
  file_len_hdr_t len_hdr = 0;

  read_items = fread(&len_hdr, sizeof(len_hdr), 1, f);

  if (1 != read_items) {
    if (feof(f)) {
      return timeline_err_eof;
    } else {
      return timeline_err_unable_to_read_entry_header;
    }
  }

  if (len_hdr > len) {
    return timeline_err_reading_entry_would_overflow;
  }

  read_items = fread(buff, 1, len_hdr, f);

  if (len_hdr != read_items) {
    return timeline_err_less_data_than_header_expects;
  }

  *lh = len_hdr;

  return timeline_ok;
}

static S timeline_save_to_disk(const T * const tl) {
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
  struct timeline_region * current_region = tl->regions;
  while(current_region) {
    for(size_t i = 0; i < current_region->used; i++) {
      struct entry_handle * e = &current_region->elems[i];
      if (timeline_ok != (wstat = write_entry(temp, e, tl->transcode_buffer, tl->transcode_buffer_len))) {
        fprintf(stderr, "Unable to write entry: %d\n", wstat);
      }
    }
    current_region = current_region->next;
  }

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

static S write_entry(FILE * f, struct entry_handle * eh, uint8_t * enc_buffer, size_t buff_len) {
  struct caut_encode_iter ei;
  enum caut_status cs;

  caut_encode_iter_init(&ei, enc_buffer, buff_len);
  if (caut_status_ok != (cs = encode_entry(&ei, &eh->entry))) {
    fprintf(stderr, "cauterize encoding returned %d\n", cs);
    return timeline_err_unable_to_encode_entry;
  }

  size_t enc_len = caut_encode_iter_used(&ei);
  file_len_hdr_t ehdr = sizeof(eh->hash) + enc_len;

  if (1 != fwrite(&ehdr, sizeof(ehdr), 1, f)) {
    return timeline_err_unable_to_write_entry_header;
  }

  if (hash_is_null(eh->hash)) {
    // no hash set, calculate one
    hash_buffer(enc_buffer, enc_len, eh->hash);
  } else {
    // a hash is set. double check it.
    hashtype_t check_hash = { 0 };
    hash_buffer(enc_buffer, enc_len, check_hash);
    if (0 != memcmp(eh->hash, check_hash, sizeof(hashtype_t))) {
      return timeline_err_hash_mismatch_before_write;
    }
  }

  if (1 != fwrite(eh->hash, sizeof(eh->hash), 1, f)) {
    return timeline_err_unable_to_write_entry_hash;
  }

  if (enc_len != fwrite(enc_buffer, 1, enc_len, f)) {
    return timeline_err_unable_to_write_encoded_entry;
  }

  return timeline_ok;
}

#undef S
#undef T
