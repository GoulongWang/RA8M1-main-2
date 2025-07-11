.syntax unified

#include "mat_vec_mul_gf256_mve.i"

.global gf256mat_prod_1936_68
.type gf256mat_prod_1936_68, %function
gf256mat_prod_1936_68:
    gf256mat_prod 1936, 68

.global gf256mat_prod_68_44
.type gf256mat_prod_68_44, %function
gf256mat_prod_68_44:
    gf256mat_prod 68, 44

.global gf256mat_prod_44_X
.type gf256mat_prod_44_X, %function
gf256mat_prod_44_X:
    gf256mat_prod_44_X 44