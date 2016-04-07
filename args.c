#include "args.h"

#include <stdio.h>
#include <string.h>

int
contains_arg(
  const int argc,
  char **argv,
  const char *arg
) {
  unsigned int i;
  
  for(i=0; i < argc; i++) {
    if(strcmp(argv[i], arg) == 0) return 1;
  }

  return 0;
}
