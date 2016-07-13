#include <stdint.h>

typedef int32_t i32;
typedef float f32;

typedef struct vi32 { i32 comps[4]; } vi32;
typedef struct mi32 { i32 comps[16]; } mi32;

void set_vi32 (vi32 *v, i32 x, i32 y, i32 z, i32 w);
void print_vi32 (const vi32 *v);
vi32 comps_to_vi32 (i32 x, i32 y, i32 z, i32 w);

void set_mi32 (mi32 *m,
  i32 m11, i32 m12, i32 m13, i32 m14,
  i32 m21, i32 m22, i32 m23, i32 m24,
  i32 m31, i32 m32, i32 m33, i32 m34,
  i32 m41, i32 m42, i32 m43, i32 m44
);

void print_mi32 (const mi32 *m);

mi32 comps_to_mi32 (
  i32 m11, i32 m12, i32 m13, i32 m14,
  i32 m21, i32 m22, i32 m23, i32 m24,
  i32 m31, i32 m32, i32 m33, i32 m34,
  i32 m41, i32 m42, i32 m43, i32 m44
);
