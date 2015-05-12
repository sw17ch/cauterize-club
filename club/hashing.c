#include "hashing.h"

#include <assert.h>
#include <gcrypt.h>

void hash_buffer(void * buffer, size_t len, hashtype_t hash) {
  int algo = GCRY_MD_SHA1;

  assert(sizeof(hashtype_t) == gcry_md_get_algo_dlen(algo));

  gcry_md_hash_buffer(algo, hash, buffer, len);
}

void hash_to_str(const hashtype_t sha1, char * str, size_t len) {
  assert(sizeof(hashtype_t) == 20);
  assert(len >= HASH_HEX_STR_LEN);

  snprintf(str, len,
      "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
      sha1[0],  sha1[1],  sha1[2],  sha1[3],
      sha1[4],  sha1[5],  sha1[6],  sha1[7],
      sha1[8],  sha1[9],  sha1[10], sha1[11],
      sha1[12], sha1[13], sha1[14], sha1[15],
      sha1[16], sha1[17], sha1[18], sha1[19]);
}
