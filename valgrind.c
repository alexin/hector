#include <stdio.h>
#include <stdlib.h>
#include "lib.h"

static vi32 p1;
static vi32 p2;
static mi32 m1;
static mi32 m2;

int main (int argc, char **argv) {
  vi32_set_vi32(&p1, vi32_from_comps(1, 2, 3, 1));
  vi32_set_vi32(&p2, vi32_add_vi32((p1), (vi32_from_comps(1, 1, 1, 1))));
  mi32_identity(&m1);
  mi32_set_mi32(&m2, mi32_add_mi32((m1), (m1)));
  vi32_print(vi32_add_vi32((p1), (p2)));
  mi32_print(m2);
  return EXIT_SUCCESS;
}
