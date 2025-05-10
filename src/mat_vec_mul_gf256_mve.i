#include "vec_scalar_mul_gf256_mve.i"

.macro gf256v_set_zero_mve c, tmp0, tmp_vec0, n_A_vec_byte
    vmov.u8 \tmp_vec0, #0

    .set cnt, (\n_A_vec_byte >> 4)
    .rept cnt
    vstrb.u8 \tmp_vec0, [\c], #16
    .endr

    // 處理餘數
    .set remainder, ((\n_A_vec_byte) & 0xF)
    .if remainder
    movs \tmp0, remainder 
    vctp.8 \tmp0  // 建立 predicate
    vpst                             
    vstrbt.u8 \tmp_vec0, [\c]
    add \c, \c, remainder
    .endif

    sub \c, \c, \n_A_vec_byte
.endm

.macro gf256v_madd_mve c, matA, gf256_b, n_A_vec_byte, tmp0, tmp1, tmp2, tmp_vec0, tmp_vec1, mask_vec, mask_vec2
    .set cnt, (\n_A_vec_byte >> 4)
    .rept cnt
    vldrb.u8 \tmp_vec0, [\matA], #16
    mov \tmp0, \gf256_b

    vec_scalar_mul_gf256 \tmp_vec1, \tmp_vec0, \tmp0, \mask_vec, \mask_vec2, \tmp1

    vldrb.u8 \tmp_vec0, [\c]
    veor.u8 \tmp_vec1, \tmp_vec1, \tmp_vec0
    vstrb.u8 \tmp_vec1, [\c], #16
    .endr

    // 處理餘數 < 16 bytes
    .set remainder, ((\n_A_vec_byte) & 0xF)
    .if remainder
    // 建立 predicate：VPR 的前 remainder 個 lane = 1
    // p0 = {1…1,0…0}
    movs \tmp0, remainder 
    vctp.8 \tmp0 
    vpst
    vldrbt.u8 \tmp_vec0, [\matA]
    mov \tmp0, \gf256_b
    vec_scalar_mul_gf256 \tmp_vec1, \tmp_vec0, \tmp0, \mask_vec, \mask_vec2, \tmp1

    vpsttt
    vldrbt.u8 \tmp_vec0, [\c]
    veort.u8 \tmp_vec1, \tmp_vec1, \tmp_vec0
    vstrbt.u8 \tmp_vec1, [\c]

    add \c, \c, remainder
    add \matA, \matA, remainder
    .endif

    sub \c, \c, \n_A_vec_byte
.endm

// 68 = 4 * 16 + 4
.macro gf256mat_prod n_A_vec_byte, n_A_width
	push {r4-r7}
	@ r0 = uint8_t *c
	@ r1 = const uint8_t *matA
	@ r2 = const uint8_t *b

	gf256v_set_zero_mve r0, r4, q0, \n_A_vec_byte
    vmov.u8 q3, #0x1b // mask2

    mov r4, \n_A_width
0:
 	ldrb r3, [r2], #1 
 	gf256v_madd_mve r0, r1, r3, \n_A_vec_byte, r5, r6, r7, q0, q1, q2, q3 
    subs r4, r4, #1
 	bne 0b

	pop {r4-r7}
    bx lr
.endm

.macro gf256mat_prod_44_X n_A_vec_byte
    push {r4-r7}
	@ r0 = uint8_t *c
	@ r1 = const uint8_t *matA
	@ r2 = const uint8_t *b
    @ r3 = n_A_width

	gf256v_set_zero_mve r0, r4, q0, \n_A_vec_byte
    vmov.u8 q3, #0x1b // mask2

    mov r4, r3
0:
 	ldrb r3, [r2], #1 
 	gf256v_madd_mve r0, r1, r3, \n_A_vec_byte, r5, r6, r7, q0, q1, q2, q3 
    subs r4, r4, #1
 	bne 0b

	pop {r4-r7}
    bx lr
.endm