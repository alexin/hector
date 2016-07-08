#include "args.h"

#include <stdio.h>
#include <string.h>

int contains_arg (int argc, char **argv, const char *arg) {
  int i;
  for (i=0; i < argc; i++) {
    if (strcmp(argv[i], arg) == 0) return 1;
  }
  return 0;
}

char* get_file (int argc, char **argv) {
  int i;
  for (i=1; i < argc; i++) {
    if (argv[i][0] != '\0' && argv[i][0] != '-') return argv[i];
  }
  return NULL;
}
