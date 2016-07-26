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

/*----------------------------------------------------------------------------*/

void vi32_set_comps (vi32 *v, i32 x, i32 y, i32 z, i32 w) {
  SX(v,x) SY(v,y) SZ(v,z) SW(v,w);
}

void vi32_set_vi32 (vi32 *v, vi32 o) {
  int i; for (i=0; i < 4; i++) v->comps[i] = o.comps[i];
}

vi32 vi32_from_comps (i32 x, i32 y, i32 z, i32 w) {
  vi32 v;
  vi32_set_comps(&v, x, y, z, 1);
  return v;
}

void vi32_zero (vi32 *v) {
  vi32_set_comps(v, 0, 0, 0, 1);
}

void vi32_print (vi32 v) {
  printf("(%d,%d,%d,%d)\n", GX(&v), GY(&v), GZ(&v), GW(&v));
}

/*----------------------------------------------------------------------------*/

void mi32_set_comps (mi32 *m,
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

void mi32_set_mi32 (mi32 *m, mi32 o) {
  int i; for (i=0; i < 16; i++) m->comps[i] = o.comps[i];
}

mi32 mi32_from_comps (
  i32 m11, i32 m12, i32 m13, i32 m14,
  i32 m21, i32 m22, i32 m23, i32 m24,
  i32 m31, i32 m32, i32 m33, i32 m34,
  i32 m41, i32 m42, i32 m43, i32 m44
) {
  mi32 m;
  mi32_set_comps(&m,
    m11, m12, m13, m14,
    m21, m22, m23, m24,
    m31, m32, m33, m34,
    m41, m42, m43, m44
  );
  return m;
}

void mi32_identity (mi32 *m) {
  mi32_set_comps(m,
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  );
}

void mi32_print (mi32 m) {
  printf("|%d,%d,%d,%d|\n|%d,%d,%d,%d|\n|%d,%d,%d,%d|\n|%d,%d,%d,%d|\n",
    G11(&m), G12(&m), G13(&m), G14(&m),
    G21(&m), G22(&m), G23(&m), G24(&m),
    G31(&m), G32(&m), G33(&m), G34(&m),
    G41(&m), G42(&m), G43(&m), G44(&m)
  );
}

/*----------------------------------------------------------------------------*/

vi32 vi32_neg (vi32 v) {
  int i; vi32 v2;
  for (i=0; i < 3; i++) v2.comps[i] = -v.comps[i];
  SW(&v2, 1);
  return v2;
}

/*----------------------------------------------------------------------------*/

vi32 vi32_add_vi32 (vi32 lhs, vi32 rhs) {
  int i; vi32 v;
  for (i=0; i < 3; i++) v.comps[i] = lhs.comps[i] + rhs.comps[i];
  SW(&v, 1);
  return v;
}

mi32 mi32_add_mi32 (mi32 lhs, mi32 rhs) {
  int i; mi32 m;
  for (i=0; i < 16; i++) m.comps[i] = lhs.comps[i] + rhs.comps[i];
  return m;
}

vi32 vi32_sub_vi32 (vi32 lhs, vi32 rhs) {
  int i; vi32 v;
  for (i=0; i < 3; i++) v.comps[i] = lhs.comps[i] - rhs.comps[i];
  SW(&v, 1);
  return v;
}

mi32 mi32_sub_mi32 (mi32 lhs, mi32 rhs) {
  int i; mi32 m;
  for (i=0; i < 16; i++) m.comps[i] = lhs.comps[i] - rhs.comps[i];
  return m;
}

/*----------------------------------------------------------------------------*/

vi32 vi32_mult_i32 (vi32 lhs, i32 rhs) {
  int i; vi32 v;
  for (i=0; i < 3; i++) v.comps[i] = lhs.comps[i] * rhs;
  SW(&v, GW(&lhs));
  return v;
}

mi32 mi32_mult_i32 (mi32 lhs, i32 rhs) {
  int i; mi32 m;
  for (i=0; i < 16; i++) m.comps[i] = lhs.comps[i] * rhs;
  return m;
}

// post-multiplication
vi32 mi32_mult_vi32 (mi32 lhs, vi32 rhs) {
  vi32 v;
  SX(&v, G11(&lhs)*GX(&rhs) + G12(&lhs)*GY(&rhs) + G13(&lhs)*GZ(&rhs) + G14(&lhs)*GW(&rhs))
  SY(&v, G21(&lhs)*GX(&rhs) + G22(&lhs)*GY(&rhs) + G23(&lhs)*GZ(&rhs) + G24(&lhs)*GW(&rhs))
  SZ(&v, G31(&lhs)*GX(&rhs) + G32(&lhs)*GY(&rhs) + G33(&lhs)*GZ(&rhs) + G34(&lhs)*GW(&rhs))
  SW(&v, G41(&lhs)*GX(&rhs) + G42(&lhs)*GY(&rhs) + G43(&lhs)*GZ(&rhs) + G44(&lhs)*GW(&rhs))
  return v;
}

// pre-multiplication
vi32 vi32_mult_mi32 (vi32 lhs, mi32 rhs) {
  vi32 v;
  SX(&v, GX(&lhs)*G11(&rhs) + GY(&lhs)*G21(&rhs) + GZ(&lhs)*G31(&rhs) + GW(&lhs)*G41(&rhs))
  SY(&v, GX(&lhs)*G12(&rhs) + GY(&lhs)*G22(&rhs) + GZ(&lhs)*G32(&rhs) + GW(&lhs)*G42(&rhs))
  SZ(&v, GX(&lhs)*G13(&rhs) + GY(&lhs)*G23(&rhs) + GZ(&lhs)*G33(&rhs) + GW(&lhs)*G43(&rhs))
  SW(&v, GX(&lhs)*G14(&rhs) + GY(&lhs)*G24(&rhs) + GZ(&lhs)*G34(&rhs) + GW(&lhs)*G44(&rhs))
  return v;
}

mi32 mi32_mult_mi32 (mi32 lhs, mi32 rhs) {
  mi32 m;
  // row 1
  S11(&m, G11(&lhs)*G11(&rhs) + G12(&lhs)*G21(&rhs) + G13(&lhs)*G31(&rhs) + G14(&lhs)*G41(&rhs))
  S12(&m, G11(&lhs)*G12(&rhs) + G12(&lhs)*G22(&rhs) + G13(&lhs)*G32(&rhs) + G14(&lhs)*G42(&rhs))
  S13(&m, G11(&lhs)*G13(&rhs) + G12(&lhs)*G23(&rhs) + G13(&lhs)*G33(&rhs) + G14(&lhs)*G43(&rhs))
  S14(&m, G11(&lhs)*G14(&rhs) + G12(&lhs)*G24(&rhs) + G13(&lhs)*G34(&rhs) + G14(&lhs)*G44(&rhs))
  // row 2
  S21(&m, G21(&lhs)*G11(&rhs) + G22(&lhs)*G21(&rhs) + G23(&lhs)*G31(&rhs) + G24(&lhs)*G41(&rhs))
  S22(&m, G21(&lhs)*G12(&rhs) + G22(&lhs)*G22(&rhs) + G23(&lhs)*G32(&rhs) + G24(&lhs)*G42(&rhs))
  S23(&m, G21(&lhs)*G13(&rhs) + G22(&lhs)*G23(&rhs) + G23(&lhs)*G33(&rhs) + G24(&lhs)*G43(&rhs))
  S24(&m, G21(&lhs)*G14(&rhs) + G22(&lhs)*G24(&rhs) + G23(&lhs)*G34(&rhs) + G24(&lhs)*G44(&rhs))
  // row 3
  S31(&m, G31(&lhs)*G11(&rhs) + G32(&lhs)*G21(&rhs) + G33(&lhs)*G31(&rhs) + G34(&lhs)*G41(&rhs))
  S32(&m, G31(&lhs)*G12(&rhs) + G32(&lhs)*G22(&rhs) + G33(&lhs)*G32(&rhs) + G34(&lhs)*G42(&rhs))
  S33(&m, G31(&lhs)*G13(&rhs) + G32(&lhs)*G23(&rhs) + G33(&lhs)*G33(&rhs) + G34(&lhs)*G43(&rhs))
  S34(&m, G31(&lhs)*G14(&rhs) + G32(&lhs)*G24(&rhs) + G33(&lhs)*G34(&rhs) + G34(&lhs)*G44(&rhs))
  // row 4
  S41(&m, G41(&lhs)*G11(&rhs) + G42(&lhs)*G21(&rhs) + G43(&lhs)*G31(&rhs) + G44(&lhs)*G41(&rhs))
  S42(&m, G41(&lhs)*G12(&rhs) + G42(&lhs)*G22(&rhs) + G43(&lhs)*G32(&rhs) + G44(&lhs)*G42(&rhs))
  S43(&m, G41(&lhs)*G13(&rhs) + G42(&lhs)*G23(&rhs) + G43(&lhs)*G33(&rhs) + G44(&lhs)*G43(&rhs))
  S44(&m, G41(&lhs)*G14(&rhs) + G42(&lhs)*G24(&rhs) + G43(&lhs)*G34(&rhs) + G44(&lhs)*G44(&rhs))
  return m;
}
