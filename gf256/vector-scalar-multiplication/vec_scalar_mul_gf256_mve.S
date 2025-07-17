.syntax unified

#include "vec_scalar_mul_gf256_mve.i"

// The scalar will be destroyed after vector-scalar multiplication.
// If you need to use it later, make sure to preserve a copy beforehand.
// 注意 mask2 處理完後會變成 8 個 1b，不是 16 bytes
.global vec_scalar_mul_gf256
.type vec_scalar_mul_gf256, %function
vec_scalar_mul_gf256:
	push {r4}
    vldrb.u8 q0, [r0]
	vldrb.u8 q1, [r1]
	vmov.u8 q3, #0x1b // mask2
	vec_scalar_mul_gf256 q0, q1, r2, q2, q3, r3 //, r4
	vstrb.u8 q0, [r0]
	pop {r4}
    bx lr