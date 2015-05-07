#include <stdint.h>

#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>

#include "transport.h"
#include "options.h"

int main(int argc, char * argv[]) {
  struct options options;

  if (option_parse_ok == option_parse(argc, argv, &options)) {
    printf("NAME: %s\n", options.name);
  } else {
    return 1;
  }
}
