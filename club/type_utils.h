#ifndef TYPE_UTILS_H
#define TYPE_UTILS_H

#include "caut/cauterize_club.h"

#include <stdint.h>

void mk_participant(
    struct participant * p,
    const char * const name);

void get_time(uint64_t * ns_dst);

void mk_entry_body_beginning(struct entry_body * body, const char * const name_str);


#endif /* TYPE_UTILS_H */
