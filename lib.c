#include "lib.h"

#include <stdio.h>
#include <stdlib.h>

#define GX(V) ((V)->comps[0])
#define GY(V) ((V)->comps[1])
#define GZ(V) ((V)->comps[2])
#define GW(V) ((V)->comps[3])

#define SX(V,I) (V)->comps[0] = (I);
#define SY(V,I) (V)->comps[1] = (I);
#define SZ(V,I) (V)->comps[2] = (I);
#define SW(V,I) (V)->comps[3] = (I);

#define G11(M) ((M)->comps[ 0])
#define G12(M) ((M)->comps[ 1])
#define G13(M) ((M)->comps[ 2])
#define G14(M) ((M)->comps[ 3])
#define G21(M) ((M)->comps[ 4])
#define G22(M) ((M)->comps[ 5])
#define G23(M) ((M)->comps[ 6])
#define G24(M) ((M)->comps[ 7])
#define G31(M) ((M)->comps[ 8])
#define G32(M) ((M)->comps[ 9])
#define G33(M) ((M)->comps[10])
#define G34(M) ((M)->comps[11])
#define G41(M) ((M)->comps[12])
#define G42(M) ((M)->comps[13])
#define G43(M) ((M)->comps[14])
#define G44(M) ((M)->comps[15])

#define S11(M,I) (M)->comps[ 0] = (I);
#define S12(M,I) (M)->comps[ 1] = (I);
#define S13(M,I) (M)->comps[ 2] = (I);
#define S14(M,I) (M)->comps[ 3] = (I);
#define S21(M,I) (M)->comps[ 4] = (I);
#define S22(M,I) (M)->comps[ 5] = (I);
#define S23(M,I) (M)->comps[ 6] = (I);
#define S24(M,I) (M)->comps[ 7] = (I);
#define S31(M,I) (M)->comps[ 8] = (I);
#define S32(M,I) (M)->comps[ 9] = (I);
#define S33(M,I) (M)->comps[10] = (I);
#define S34(M,I) (M)->comps[11] = (I);
#define S41(M,I) (M)->comps[12] = (I);
#define S42(M,I) (M)->comps[13] = (I);
#define S43(M,I) (M)->comps[14] = (I);
#define S44(M,I) (M)->comps[15] = (I);

void vi32_set (vi32 *v, i32 x, i32 y, i32 z, i32 w) {
  SX(v,x) SY(v,y) SZ(v,z) SW(v,w);
}

void vi32_zero (vi32 *v) {
  vi32_set (v, 0, 0, 0, 1);
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

void vi32_print (vi32 v) {
  printf("(%d,%d,%d,%d)\n", GX(&v), GY(&v), GZ(&v), GW(&v));
}

vi32 vi32_from_comps (i32 x, i32 y, i32 z, i32 w) {
  vi32 v;
  vi32_set(&v, x, y, z, 1);
  return v;
}

void mi32_set (mi32 *m,
  i32 m11, i32 m12, i32 m13, i32 m14,
  i32 m21, i32 m22, i32 m23, i32 m24,
  i32 m31, i32 m32, i32 m33, i32 m34,
  i32 m41, i32 m42, i32 m43, i32 m44
) {
  S11(m,m11) S12(m,m12) S13(m,m13) S14(m,m14)
  S21(m,m21) S22(m,m22) S23(m,m23) S24(m,m24)
  S31(m,m31) S32(m,m32) S33(m,m33) S34(m,m34)
  S41(m,m41) S42(m,m42) S43(m,m43) S44(m,m44)
}

void mi32_identity (mi32 *m) {
  mi32_set(m, 1, 0, 0, 0,
              0, 1, 0, 0,
              0, 0, 1, 0,
              0, 0, 0, 1);
}

void mi32_print (mi32 m) {
  printf("|%d,%d,%d,%d|\n|%d,%d,%d,%d|\n|%d,%d,%d,%d|\n|%d,%d,%d,%d|\n",
    G11(&m), G12(&m), G13(&m), G14(&m),
    G21(&m), G22(&m), G23(&m), G24(&m),
    G31(&m), G32(&m), G33(&m), G34(&m),
    G41(&m), G42(&m), G43(&m), G44(&m)
  );
}

mi32 mi32_from_comps (
  i32 m11, i32 m12, i32 m13, i32 m14,
  i32 m21, i32 m22, i32 m23, i32 m24,
  i32 m31, i32 m32, i32 m33, i32 m34,
  i32 m41, i32 m42, i32 m43, i32 m44
) {
  mi32 m;
  mi32_set(&m,
    m11, m12, m13, m14,
    m21, m22, m23, m24,
    m31, m32, m33, m34,
    m41, m42, m43, m44
  );
  return m;
}
