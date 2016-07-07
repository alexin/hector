#include <stdint.h>

typedef int32_t i32;
typedef float f32;

typedef struct vi32 { i32 comps[4]; } vi32;

void set_vi32 (vi32 *v, i32 x, i32 y, i32 z, i32 w);
void print_vi32 (const vi32 *v);
