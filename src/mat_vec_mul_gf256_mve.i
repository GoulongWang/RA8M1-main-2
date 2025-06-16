#include "vec_scalar_mul_gf256_mve.i"

.macro gf256v_set_zero_mve c, tmp0, tmp_vec0, n_A_vec_byte
    vmov.u8 \tmp_vec0, #0

    mov \tmp0, \n_A_vec_byte
    lsr \tmp0, \tmp0, #4
1:  
    vstrb.u8 \tmp_vec0, [\c], #16
    subs \tmp0, \tmp0, #1
    bne 1b

    // 處理餘數
    mov \tmp0, \n_A_vec_byte
    and \tmp0, \tmp0, #0xF
    cbz \tmp0, 2f
    vctp.8 \tmp0  // 建立 predicate
    vpst                             
    vstrbt.u8 \tmp_vec0, [\c]
    add \c, \c, \tmp0
2:
    sub \c, \c, \n_A_vec_byte
.endm

.macro gf256v_madd_mve c, matA, gf256_b, n_A_vec_byte, tmp0, tmp1, tmp2, tmp_vec0, tmp_vec1, mask_vec, mask_vec2
    mov \tmp2, \n_A_vec_byte
    lsr \tmp2, \tmp2, #4
3:  
    vldrb.u8 \tmp_vec0, [\matA], #16
    mov \tmp0, \gf256_b
    vec_scalar_mul_gf256 \tmp_vec1, \tmp_vec0, \tmp0, \mask_vec, \mask_vec2, \tmp1
    vldrb.u8 \tmp_vec0, [\c]
    veor.u8 \tmp_vec1, \tmp_vec1, \tmp_vec0
    vstrb.u8 \tmp_vec1, [\c], #16

    subs \tmp2, \tmp2, #1
    bne 3b
    
    // 處理餘數 < 16 bytes
    mov \tmp0, \n_A_vec_byte
    and \tmp0, \tmp0, #0xF
    cmp \tmp0, #0
    beq 4f
    
    mov \tmp2, \tmp0
    vctp.8 \tmp0 
    vpst
    vldrbt.u8 \tmp_vec0, [\matA]
    add \matA, \matA, \tmp0
    mov \tmp0, \gf256_b
    vec_scalar_mul_gf256 \tmp_vec1, \tmp_vec0, \tmp0, \mask_vec, \mask_vec2, \tmp1

    vpsttt
    vldrbt.u8 \tmp_vec0, [\c]
    veort.u8 \tmp_vec1, \tmp_vec1, \tmp_vec0
    vstrbt.u8 \tmp_vec1, [\c]

    add \c, \c, \tmp2
4:  
    sub \c, \c, \n_A_vec_byte
.endm

// 68 = 4 * 16 + 4
.macro gf256mat_prod n_A_vec_byte, n_A_width
	push {r4-r8}
	@ r0 = uint8_t *c
	@ r1 = const uint8_t *matA
	@ r2 = const uint8_t *b

	gf256v_set_zero_mve r0, r4, q0, \n_A_vec_byte
    vmov.u8 q3, #0x1b // mask2

    mov r4, \n_A_width
5:
    ldrb r3, [r2], #1
 	gf256v_madd_mve r0, r1, r3, \n_A_vec_byte, r5, r6, r7, q0, q1, q2, q3 
    subs r4, r4, #1
 	bne 5b

	pop {r4-r8}
    bx lr
.endm

.macro gf256mat_prod_44_X n_A_vec_byte
    push {r4-r8}
	@ r0 = uint8_t *c
	@ r1 = const uint8_t *matA
	@ r2 = const uint8_t *b
    @ r3 = n_A_width

	gf256v_set_zero_mve r0, r4, q0, \n_A_vec_byte
    vmov.u8 q3, #0x1b // mask2

    mov r4, r3
6:
 	ldrb r3, [r2], #1 
 	gf256v_madd_mve r0, r1, r3, \n_A_vec_byte, r5, r6, r7, q0, q1, q2, q3 
    subs r4, r4, #1
 	bne 6b

	pop {r4-r8}
    bx lr
.endm

//

// size = 16 * cnt + remainder
.macro memcpy_mve dst_ptr, src_ptr, size, tmp0, tmp_vec
    cmp \size, #0
    beq 4f

    lsr \tmp0, \size, #4
    wls lr, \tmp0, 4f
3:  
    vldrb.u8 \tmp_vec, [\src_ptr], #16
    vstrb.u8 \tmp_vec, [\dst_ptr], #16
    le lr, 3b

    // < 16 bytes
    and \tmp0, \size, #0xF
    .set remainder, (\tmp0)
    vctp.8 \tmp0  // 建立 predicate
    vpstt     
    vldrbt.u8 \tmp_vec, [\src_ptr]                       
    vstrbt.u8 \tmp_vec, [\dst_ptr]
    add \dst_ptr, \dst_ptr, \tmp0
    add \src_ptr, \src_ptr, \tmp0
4:
    sub \dst_ptr, \dst_ptr, \size
.endm

// SIZE_BATCH = 44
.macro memset_mve ptr, value, zero_vec
    vstrb.u8 \zero_vec, [\ptr], #16
    vstrb.u8 \zero_vec, [\ptr], #16
    sub \value, \value, #32
    vctp.8 \value
    vpst
    vstrbt.u8 \zero_vec, [\ptr], #12
.endm

.macro gf256v_add_mve accu_b, a, tmp0, tmp_vec0, tmp_vec1
    vldrb.u8 \tmp_vec0, [\accu_b]
    vldrb.u8 \tmp_vec1, [\a]
    veor.u8 \tmp_vec0, \tmp_vec0, \tmp_vec1
    vstrb.u8 \tmp_vec0, [\accu_b], #16

    vldrb.u8 \tmp_vec0, [\accu_b]
    vldrb.u8 \tmp_vec1, [\a, #16]
    veor.u8 \tmp_vec0, \tmp_vec0, \tmp_vec1
    vstrb.u8 \tmp_vec0, [\accu_b], #16

    mov \tmp0, #12
    vctp.8 \tmp0
    vpstttt
    vldrbt.u8 \tmp_vec0, [\accu_b]
    vldrbt.u8 \tmp_vec1, [\a, #32]
    veort.u8 \tmp_vec0, \tmp_vec0, \tmp_vec1
    vstrbt.u8 \tmp_vec0, [\accu_b], #12
.endm

.macro gf256mat_prod_test c, matA, b, n_A_vec_byte, n_A_width, tmp0, tmp1, tmp2, tmp3, tmp4, mask_vec0, tmp_vec0, tmp_vec1, tmp_vec2
	gf256v_set_zero_mve \c, \tmp0, \tmp_vec0, \n_A_vec_byte

    mov \tmp0, \n_A_width
6:
 	ldrb \tmp1, [\b], #1
    gf256v_madd_mve \c, \matA, \tmp1, \n_A_vec_byte, \tmp2, \tmp3, \tmp4, \tmp_vec0, \tmp_vec1, \tmp_vec2, \mask_vec0 
    subs \tmp0, \tmp0, #1
 	bne 6b
.endm