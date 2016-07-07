#include "lib.h"

#include <stdlib.h>
#include <stdio.h>

#define GX(V) (V->comps[0])
#define GY(V) (V->comps[1])
#define GZ(V) (V->comps[2])
#define GW(V) (V->comps[3])

#define SX(V,I) (V->comps[0] = I)
#define SY(V,I) (V->comps[1] = I)
#define SZ(V,I) (V->comps[2] = I)
#define SW(V,I) (V->comps[3] = I)

void set_vi32 (vi32 *v, i32 x, i32 y, i32 z, i32 w) {
  SX(v,x); SY(v,y); SZ(v,z); SW(v,w);
}

/*i32 dot (const vi32 *a, const vi32 *b) {
  return GX(a)*GX(b) + GY(a)*GY(b) + GZ(a)*GZ(b) + GW(a)*GW(b);
}

vi32 cross (const vi32 *a, const vi32 *b) {
  vi32 r;
  set_vi32(&r,
    GY(a)*GZ(b) - GZ(a)*GY(b),
    GZ(a)*GX(b) - GX(a)*GZ(b),
    GX(a)*GY(b) - GY(a)*GX(b),
    1
  );
  return r;
}*/

void print_vi32 (const vi32 *v) {
  printf("(%d,%d,%d,%d)\n", GX(v), GY(v), GZ(v), GW(v));
}
