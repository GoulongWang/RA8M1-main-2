.syntax unified

#include "vec_scalar_mul_gf16_mve.i"

.global mve_mul_gf16
.type mve_mul_gf16, %function
mve_mul_gf16:
	push {r4}
	vldrb.u8 q0, [r1]
	vmov.u8 q7, #0 // 把 acc 歸零
	vdup.u8 q1, r2
	vmov.u8 q2, #0x0f
	vmov.u8 q3, #3
	vec_scalar_mul q7, q0, q1, q2, q3, q4, q5, q6
	vstrb.u8 q7, [r0] 
	pop {r4}
    bx lr