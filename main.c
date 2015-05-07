#include <stdint.h>

#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>

#include "transport.h"
#include "options.h"

int main(int argc, char * argv[]) {
  struct options options;

  if (option_parse_ok == option_parse(argc, argv, &options)) {
    printf("NAME: %s\n", options.name);
    for(size_t i = 0; i < options.peer_set.count; i++){
      struct address * a = &options.peer_set.addresses[i];
      printf("Address: %s\n", a->elems);
    }
  } else {
    return 1;
  }
}
