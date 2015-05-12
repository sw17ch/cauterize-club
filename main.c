#include <stdint.h>

#include "club/network.h"
#include "club/options.h"
#include "club/app.h"

int main(int argc, char * argv[]) {
  struct options * options;

  if (option_parse_ok == option_parse(argc, argv, &options)) {
    int r = run_with_options(options);
    options_free(options);
    return r;
  } else {
    return 1;
  }
}

