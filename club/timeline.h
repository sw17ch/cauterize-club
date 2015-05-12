#ifndef TIMELINE_H
#define TIMELINE_H

struct timeline;

enum timeline_status {
  timeline_ok,
  timeline_err_path_not_readable,
  timeline_err_header_not_enough_data,
  timeline_err_header_mismatch,
  timeline_err_path_not_writeable,
  timeline_err_unable_to_rename_temp,
  timeline_err_unable_to_write_header,
  timeline_err_unable_to_encode_entry,
  timeline_err_unable_to_write_encoded_entry,
  timeline_err_unable_to_read_entry_header,
  timeline_err_reading_entry_would_overflow,
  timeline_err_less_data_than_header_expects,
};

enum timeline_status timeline_init_from_file(
    const char * const path,
    struct timeline ** const tl);

enum timeline_status timeline_deinit(
    struct timeline * const tl);

#endif /* TIMELINE_H */
