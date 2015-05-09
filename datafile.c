#include "datafile.h"
#include "caut/cauterize_club.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

enum write_status {
  write_status_ok,
  write_status_err_header,
};

static bool file_readable(const char * const path, int * const eno);
static bool file_writeable(const char * const path, int * const eno);

static enum write_status write_header(FILE * f);

enum datafile_status load_records_from_file(const char * const path, struct record_set * const set) {
  assert(path);
  assert(set);

  set->count = 0;
  set->records = NULL;

  int eno = 0;
  if (!file_readable(path, &eno)) {
    if (ENOENT != eno) {
      fprintf(stderr, "Unable to read file `%s`. Reason given: %s\n", path, strerror(eno));
      return datafile_err;
    }
  } else {
    /* The file exists. Load it to memory. */
    assert(false);
  }

  return datafile_ok;
}

enum datafile_status write_records_to_file(const char * const path, const struct record_set * const set) {
  char temp_path[] = "/tmp/cauterize-club.tmp.XXXXXXXXXXXXXXXX";
  int fd = mkstemp(temp_path);
  FILE * temp = fdopen(fd, "w");

  if (write_status_ok != write_header(temp)) {
    fputs("Unable to write header.\n", stderr);
  }

  fclose(temp);

  if (0 != rename(temp_path, path)) {
    int e = errno;
    fprintf(stderr, "Unable rename temp record file: %s\n", strerror(e));
    return datafile_err;
  } else {
    return datafile_ok;
  }
}

static bool file_readable(const char * const path, int * const eno) {
  int a = access(path, R_OK);
  if (NULL != eno) { *eno = errno; }
  return (0 == a);
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
