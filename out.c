#include <stdio.h>
#include <stdlib.h>
#include "lib.h"

static vi32 p;

int main (int argc, char **argv) {
  set_vi32(&p, 1, 2, 3, 1);
  print_vi32(&p);
  return EXIT_SUCCESS;
}
