#include <stdint.h>

#include "transport.h"
#include "options.h"

int main(int argc, char * argv[]) {
  struct options * options = NULL;

  if (option_parse_ok == option_parse(argc, argv, &options)) {
    return 0;
  } else {
    return 1;
  }
}
