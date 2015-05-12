#include "datafile.h"
#include "hashing.h"
#include "caut/cauterize_club.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

struct timeline {
  size_t count;
  struct message_cauterize_club * entries;
};

enum read_status {
  read_status_ok,
  read_status_err_header_not_enough_data,
  read_status_err_header_mismatch,
};

enum write_status {
  write_status_ok,
  write_status_err_header,
  write_status_err_entry_encode,
  write_status_err_entry_write,
};

static bool file_readable(const char * const path, int * const eno);
static enum read_status read_and_validate_header(FILE * f);

static bool file_writeable(const char * const path, int * const eno);
static enum write_status write_header(FILE * f);
static enum write_status write_entry(FILE * f, struct message_cauterize_club * m);

enum datafile_status load_timeline_from_file(const char * const path, struct timeline ** const tl) {
  struct timeline * timeline = calloc(1, sizeof(*tl));
  *tl = timeline;

  timeline->count = 0;
  timeline->entries = NULL;

  int eno = 0;
  if (!file_readable(path, &eno)) {
    if (ENOENT != eno) {
      fprintf(stderr, "Unable to read file `%s`. Reason given: %s\n", path, strerror(eno));
      return datafile_err;
    }
  } else {
    FILE * datafile = fopen(path, "r");
    {
      enum read_status stat;

      if (read_status_ok != (stat = read_and_validate_header(datafile))) {
        fprintf(stderr, "Unable to validate datafile header: %d\n", stat);
        return datafile_err;
      }

      // Header validated. Read in timeline.
      // TODO: read entries
    }
    fclose(datafile);
  }

  return datafile_ok;
}

enum datafile_status write_timeline_to_file(const char * const path, const struct timeline * const tl) {
  // TODO: Using rename like this only works if /tmp and path are on the same
  // file system. Try and make the temp file beside parent directory of `path`.

  char temp_path[] = "/tmp/cauterize-club.tmp.XXXXXXXXXXXXXXXX";
  int fd = mkstemp(temp_path);
  FILE * temp = fdopen(fd, "w");
  enum write_status wstat;

  int eno = 0;
  if (!file_writeable(path, &eno)) {
    if (ENOENT != eno) {
      fprintf(stderr, "Unable to write file `%s`. Reason given: %s\n", path, strerror(eno));
      return datafile_err;
    }
  }

  if (write_status_ok != write_header(temp)) {
    fputs("Unable to write header.\n", stderr);
    return datafile_err;
  }

  // Header written. Write timeline.
  for (size_t i = 0; i < tl->count; i++) {
    if (write_status_ok != (wstat = write_entry(temp, &tl->entries[i]))) {
      fprintf(stderr, "Unable to write entry: %d\n", wstat);
    }
  }

  fclose(temp);

  if (0 != rename(temp_path, path)) {
    int e = errno;
    fprintf(stderr, "Unable rename temp timeline file: %s\n", strerror(e));
    return datafile_err;
  } else {
    return datafile_ok;
  }
}

enum datafile_status free_timeline(struct timeline * tl) {
  free(tl);
  return datafile_ok;
}

static bool file_readable(const char * const path, int * const eno) {
  int a = access(path, R_OK);
  if (NULL != eno) { *eno = errno; }
  return (0 == a);
}

static enum read_status read_and_validate_header(FILE * f) {
  hashtype_t schema_hash = { 0 };
  size_t read_items = fread(schema_hash, sizeof(schema_hash), 1, f);

  if (1 != read_items) {
    return read_status_err_header_not_enough_data;
  } else {
    if (0 != memcmp(schema_hash, SCHEMA_HASH_cauterize_club, sizeof(schema_hash))) {
      char hash_exp[HASH_HEX_STR_LEN] = { 0 };
      char hash_act[HASH_HEX_STR_LEN] = { 0 };

      hash_to_str(SCHEMA_HASH_cauterize_club, hash_exp, sizeof(hash_exp));
      hash_to_str(schema_hash, hash_act, sizeof(hash_act));

      fprintf(stderr, "Hash mismatch:\n"
                      "  Expected: %s\n"
                      "  Actual:   %s\n", hash_exp, hash_act);

      return read_status_err_header_mismatch;
    } else {
      return read_status_ok;
    }
  }
}

static bool file_writeable(const char * const path, int * const eno) {
  int a = access(path, W_OK);
  if (NULL != eno) { *eno = errno; }
  return (0 == a);
}

static enum write_status write_header(FILE * f) {
  size_t written =
    fwrite(SCHEMA_HASH_cauterize_club, sizeof(SCHEMA_HASH_cauterize_club), 1, f);

  if (1 != written) {
    return write_status_err_header;
  } else {
    return write_status_ok;
  }
}

static enum write_status write_entry(FILE * f, struct message_cauterize_club * m) {
  enum caut_status cstat = caut_status_ok;

  // TODO: This can be the max size of the 
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
    return write_status_err_entry_encode;
  }

  size_t len = caut_encode_iter_used(&enc_iter);
  printf("len %lu\n", len);

  if (len != fwrite(enc_buffer, sizeof(uint8_t), len, f)) {
    return write_status_err_entry_write;
  } else {
    return write_status_ok;
  }
}
