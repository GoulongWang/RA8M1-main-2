.macro gf256v_add_mve accu_b, a, tmp_vec0, tmp_vec1
    vldrb.u8 \tmp_vec0, [\accu_b]
    vldrb.u8 \tmp_vec1, [\a]
    veor.u8 \tmp_vec0, \tmp_vec0, \tmp_vec1
    vstrb.u8 \tmp_vec0, [\accu_b], #16

    vldrb.u8 \tmp_vec0, [\accu_b]
    vldrb.u8 \tmp_vec1, [\a, #16]
    veor.u8 \tmp_vec0, \tmp_vec0, \tmp_vec1
    vstrb.u8 \tmp_vec0, [\accu_b], #16
.endm

.macro memset_to_zero ptr, zero_vec
    vstrb.u8 \zero_vec, [\ptr], #16
    vstrb.u8 \zero_vec, [\ptr], #16
.endm

.macro memcpy_mve dst_ptr, src_ptr, size, tmp0, tmp_vec
    cmp \size, #0
    beq 3f

    lsr \tmp0, \size, #5
    wls lr, \tmp0, 3f
4:
    vldrb.u8 \tmp_vec, [\src_ptr], #16
    vstrb.u8 \tmp_vec, [\dst_ptr], #16
    vldrb.u8 \tmp_vec, [\src_ptr], #16
    vstrb.u8 \tmp_vec, [\dst_ptr], #16
    le lr, 4b
3:
.endm

.macro gf16mat_prod_test c, matA, b, n_A_vec_byte, n_A_width, tmp0, tmp1, tmp2, bb_vec, mask_vec0, mask_vec1, tmp_vec1, tmp_vec2, tmp_vec3, tmp_vec4, tmp_vec5
    vmov.u8 \tmp_vec1, #0        
    vstrb.u8  \tmp_vec1, [\c]
    vstrb.u8  \tmp_vec1, [\c, #16]
    
    mov \tmp0, #0
6:
    gf16v_get_ele_mve \tmp2, \b, \tmp0, \tmp1
    vdup.u8 \bb_vec, \tmp2
    gf16v_madd_mve \c, \matA, \n_A_vec_byte, \tmp1, \bb_vec, \mask_vec0, \mask_vec1, \tmp_vec1, \tmp_vec2, \tmp_vec3, \tmp_vec4, \tmp_vec5
    sub \c, \c, \n_A_vec_byte

    add \tmp0, \tmp0, #1
    cmp \tmp0, \n_A_width 
    bne 6b
.endm