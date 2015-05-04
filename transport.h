#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "caut/cauterize_club.h"

struct header {
  hashtype_t schema_hash;
  uint8_t buffer[MAX_SIZE_cauterize_club];
};



#endif /* TRANSPORT_H */
