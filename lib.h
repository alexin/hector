#include <stdint.h>

typedef int32_t i32;
typedef float f32;

typedef struct vi32 { i32 comps[4]; } vi32;
typedef struct mi32 { i32 comps[16]; } mi32;

void vi32_set (vi32 *v, i32 x, i32 y, i32 z, i32 w);
void vi32_zero (vi32 *v);
void vi32_print (vi32 v);
vi32 vi32_from_comps (i32 x, i32 y, i32 z, i32 w);

void mi32_set (mi32 *m,
  i32 m11, i32 m12, i32 m13, i32 m14,
  i32 m21, i32 m22, i32 m23, i32 m24,
  i32 m31, i32 m32, i32 m33, i32 m34,
  i32 m41, i32 m42, i32 m43, i32 m44
);
void mi32_identity (mi32 *m);
void mi32_print (mi32 m);
mi32 mi32_from_comps (
  i32 m11, i32 m12, i32 m13, i32 m14,
  i32 m21, i32 m22, i32 m23, i32 m24,
  i32 m31, i32 m32, i32 m33, i32 m34,
  i32 m41, i32 m42, i32 m43, i32 m44
);
