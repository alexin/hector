#include "args.h"

#include <stdio.h>
#include <string.h>

int contains_arg (int argc, char **argv, const char *arg) {
  u8 i;
  for (i=0; i < argc; i++) {
    if (strcmp(argv[i], arg) == 0) return 1;
  }
  return 0;
}

char* get_file (int argc, char **argv) {
  return NULL;
}
