// This macro processes exactly 16 bytes.
.macro vec_scalar_mul_gf256 out_vec, in_vec, scalar, mask_vec, mask_vec2, tmp0
    ands \tmp0, \scalar, #1
    neg \tmp0, \tmp0
    vdup.u8 \mask_vec, \tmp0
    vand \out_vec, \in_vec, \mask_vec

    //mov \tmp1, #7
//0:
    .rept 7
    vshr.u8 \mask_vec, \in_vec, #7 // 直接把 in_vec 右移 7 bit 就可得到 MSB Vector 了，每個 element 中為 0/1
    vshl.u8 \in_vec,  \in_vec, #1  // in_vec <<= 1
    vmul.u8 \mask_vec, \mask_vec, \mask_vec2 // 對每個 MSB 元素乘上 0x1b
    veor.u8 \in_vec,  \in_vec, \mask_vec // a32 = (a32 << 1) ^ ((a_msb >> 7) * 0x1b);

    // neg 代表取負數，若 bit = 0，取負數 = 0 - 0 = 0; 若 bit = 1，取負數 = 0 - 1 = -1 (拿來當作 mask)
    // 若 bit = ((scalar >> 1) & 1) = 0，in_vec[i] 清 0
    // 若 bit = 1，in_vec[i] 全部保留，利用遮罩 0xFF
    lsr \scalar, \scalar, #1
    ands \tmp0, \scalar, #1
    neg \tmp0, \tmp0
    vdup.u8 \mask_vec, \tmp0
    vand \mask_vec, \mask_vec, \in_vec
    veor.u8 \out_vec, \out_vec, \mask_vec
    .endr
    //subs \tmp1, \tmp1, #1
    //bne 0b
.endm

.macro gf256_vector_scalar inout_ptr, gf256_b, num_bytes, tmp0, tmp1, tmp2, out_vec, in_vec, mask_vec, mask_vec2
    lsr \tmp0, \num_bytes, #4
    cmp \tmp0, #0
    beq 10f
11:    
	vldrb.u8 \in_vec, [\inout_ptr]
    mov \tmp2, \gf256_b

    vmov.u8 \mask_vec2, #0x1b // mask2
    vec_scalar_mul_gf256 \out_vec, \in_vec, \tmp2, \mask_vec, \mask_vec2, \tmp1
    vstrb.u8 \out_vec, [\inout_ptr], #16

    subs \tmp0, \tmp0, #1
    bne 11b

10:
    // process  < 16 bytes
    and \tmp0, \num_bytes, #0xF
    cmp \tmp0, #0
    beq 12f
    
    vctp.8 \tmp0
    vpst
    vldrbt.u8 \in_vec, [\inout_ptr]

    mov \tmp2, \gf256_b
    vmov.u8 \mask_vec2, #0x1b // mask2
    vec_scalar_mul_gf256 \out_vec, \in_vec, \tmp2, \mask_vec, \mask_vec2, \tmp1
    
    vpst
    vstrbt.u8 \out_vec, [\inout_ptr]
    add \inout_ptr, \inout_ptr, \tmp0
12: 
.endm