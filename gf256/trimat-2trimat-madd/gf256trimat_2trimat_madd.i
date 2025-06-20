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