#ifndef TIMELINE_INTERNAL_H
#define TIMELINE_INTERNAL_H

#include "timeline.h"
#include "caut/cauterize_club.h"

#include <stddef.h>
#include <stdint.h>

struct timeline_region {
  // Where elements for this node are stored.
  struct entry_handle * elems;

  // The next region.
  struct timeline_region * next;

  // How many entries in the allocated region are used.
  size_t used;

  // How many entries fit in the allocated region.
  size_t count;
};

struct timeline {
  struct timeline_region * regions;
  const char * file_path;
  uint8_t * transcode_buffer;
  size_t transcode_buffer_len;
};

typedef uint32_t file_len_hdr_t;

#endif /* TIMELINE_INTERNAL_H */
