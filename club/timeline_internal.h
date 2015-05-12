#ifndef TIMELINE_INTERNAL_H
#define TIMELINE_INTERNAL_H

#include "timeline.h"
#include "caut/cauterize_club.h"

#include <stddef.h>

struct entry_handle {
  hashtype_t hash;
  struct entry entry;
};

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
  void * transcode_buffer;
  const char * file_path;
  uint8_t filler[128];
};

#endif /* TIMELINE_INTERNAL_H */
