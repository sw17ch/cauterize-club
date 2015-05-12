#ifndef NETWORK_H
#define NETWORK_H

#include "caut/cauterize_club.h"

struct header {
  hashtype_t schema_hash;
  uint8_t buffer[MAX_SIZE_cauterize_club];
};



#endif /* NETWORK_H */
