#ifndef TIMELINE_H
#define TIMELINE_H

struct timeline;
struct entry_iter;

enum timeline_status {
  timeline_ok = 0,

  /* reading */
  timeline_err_eof = 100,
  timeline_err_path_not_readable,
  timeline_err_header_not_enough_data,
  timeline_err_header_mismatch,
  timeline_err_unable_to_read_entry_header,
  timeline_err_reading_entry_would_overflow,
  timeline_err_less_data_than_header_expects,
  timeline_err_unable_to_decode_entry,

  /* writing */
  timeline_err_path_not_writeable = 200,
  timeline_err_unable_to_rename_temp,
  timeline_err_unable_to_write_header,
  timeline_err_unable_to_encode_entry,
  timeline_err_unable_to_write_entry_header,
  timeline_err_unable_to_write_entry_hash,
  timeline_err_unable_to_write_encoded_entry,
};

struct entry_handle {
  hashtype_t hash;
  struct entry entry;
};

enum timeline_status timeline_init_from_file(
    const char * const path,
    struct timeline ** const tl);

enum timeline_status timeline_deinit(
    struct timeline * const tl);

struct entry_handle * timeline_new_entry(
    struct timeline * const tl);

const struct entry_handle * timeline_last_entry(
    const struct timeline * const tl);


struct entry_iter * timeline_iterator(
    const struct timeline * const tl);

void timeline_iterator_reset(
    struct entry_iter * iter);

void timeline_iterator_next(
    struct entry_iter * iter);

bool timeline_iterator_done(
    struct entry_iter * iter);

const struct entry_handle * timeline_iterator_entry(
    const struct entry_iter * const iter);

void timeline_iterator_free(
    struct entry_iter * iter);

#endif /* TIMELINE_H */
