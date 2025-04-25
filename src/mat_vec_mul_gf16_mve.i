#include "vec_scalar_mul_gf16_mve.i"

.macro gf256v_set_zero_mve c, tmp0, tmp_vec0
    mov \tmp0, #0

    .rept 128
    vdup.u8 \tmp_vec0, \tmp0
    vldrb.u8 \tmp_vec0, [\c], #16
    .endr

    sub \c, \c, #2048
.endm

.macro gf16v_get_ele_mve out, vec_ptr,index, tmp0
    lsrs \tmp0, \index, #1
    ldrb \out, [\vec_ptr, \tmp0]
    ands \tmp0, \index, #1
    ite eq
    andeq \out, \out, #0x0f // == 0 => 取下半 nibble
    lsrne \out, \out, #4    // == 1 => 取上半 nibble
.endm

.macro gf16v_madd_mve c, matA, bb, tmp0, tmp_vec0, mask1_vec, mask2_vec, tmp_vec3, tmp_vec4, tmp_vec5, tmp_vec6, tmp_vec7
    .rept 128

    vldrb.u8 \tmp_vec0, [\matA], #16

    mov \tmp0, #0      
	vdup.u8 \tmp_vec7, \tmp0 // 把 acc 歸零

	mov \tmp0, #0x0f
	vdup.u8 \mask1_vec, \tmp0

	mov \tmp0, #3
	vdup.u8 \mask2_vec, \tmp0
    
    vec_scalar_mul \tmp_vec7, \tmp_vec0, \bb, \tmp0, \mask1_vec, \mask2_vec, \tmp_vec3, \tmp_vec4, \tmp_vec5, \tmp_vec6
    
    vldrb.u8 \tmp_vec0, [\c]
    veor.u8 \tmp_vec7, \tmp_vec7, \tmp_vec0
    vstrb.u8 \tmp_vec7, [\c], #16
    .endr
.endm



