#ifndef HASHING_H
#define HASHING_H

#include "caut/cauterize_club.h"
#include <stdbool.h>

#define HASH_HEX_STR_LEN (41)

enum hashing_status {
  hashing_ok,
  hashing_err,
};

void hash_buffer(void * buffer, size_t len, hashtype_t hash);
void hash_to_str(const hashtype_t hash, char * str, size_t len);
bool hash_is_null(hashtype_t hash);

#endif /* HASHING_H */
