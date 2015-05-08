#include <stdint.h>

#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>

#include "network.h"
#include "options.h"
#include "app.h"

int main(int argc, char * argv[]) {
  struct options options;

  if (option_parse_ok == option_parse(argc, argv, &options)) {
    return run_with_options(&options);
  } else {
    return 1;
  }
}
