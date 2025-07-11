.syntax unified

#include "mat_vec_mul_gf16_mve.i"

.global gf16mat_prod_2048_96
.type gf16mat_prod_2048_96, %function
gf16mat_prod_2048_96:
    gf16mat_prod 2048, 96

.global gf16mat_prod_48_64
.type gf16mat_prod_48_64, %function
gf16mat_prod_48_64:
    gf16mat_prod 48, 64

.global gf16mat_prod_32_X
.type gf16mat_prod_32_X, %function
gf16mat_prod_32_X:
    gf16mat_prod_32_X 32