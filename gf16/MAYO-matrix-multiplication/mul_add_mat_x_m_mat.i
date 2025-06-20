#include "../vector-scalar-multiplication/vec_scalar_mul_gf16_mve.i"

.macro mul_table_mve inout, tmp0
    ldr \tmp0, =0x08040201
    mul \inout, \inout, \tmp0

    ldr \tmp0, =0xf0f0f0f0
    and \tmp0, \inout, \tmp0
    lsr \tmp0, \tmp0, #3    

    eor \inout, \inout, \tmp0
    lsr \tmp0, \tmp0, #1 
    eor \inout, \inout, \tmp0
.endm
 
.macro m_vec_mul_add_mve in, a, acc, tmp0, tmp1, tmp2, mask_vec, mask2_vec, tmp_vec0, tmp_vec1, tmp_vec2, tmp_vec3, tmp_vec4, tmp_vec5
    mul_table_mve \a, \tmp0 // a 的結果會變成 tab

    mov \tmp0, #0
3:
    vldrw.u32 \tmp_vec5, [\acc]
    
    // ( in[i]       & lsb_ask) * (tab & 0xf)
    vldrb.u8 \tmp_vec0, [\in]               // q1 = in[i]
    mov \tmp2, #1
    vdup.u8 \tmp_vec1, \tmp2                // load lsb_ask
    vand \tmp_vec0, \tmp_vec0, \tmp_vec1    // in[i] & lsb_ask
    and \tmp1, \a, #0xf // tab & 0xf

    vdup.u8 \tmp_vec1, \tmp1 // scalar mask
    mov \tmp2, #0x0f
	vdup.u8 \mask_vec, \tmp2
	mov \tmp2, #3
	vdup.u8 \mask2_vec, \tmp2

    vec_scalar_mul \tmp_vec5, \tmp_vec0, \tmp_vec1, \mask_vec, \mask2_vec, \tmp_vec2, \tmp_vec3, \tmp_vec4
    // acc[i] ^= (in[i] & lsb_ask) * (tab & 0xf)
     
    // ^ ((in[i] >> 1) & lsb_ask) * ((tab >> 8)  & 0xf)
    vldrb.u8 \tmp_vec0, [\in]
    vshr.u8 \tmp_vec0, \tmp_vec0, #1
    mov \tmp2, #1
    vdup.u8 \tmp_vec1, \tmp2  
    vand \tmp_vec0, \tmp_vec0, \tmp_vec1
    lsr \tmp1, \a, #8
    and \tmp1, \tmp1, #0xf  // tmp1 = scalar

    vdup.u8 \tmp_vec1, \tmp1 // scalar mask
    mov \tmp2, #0x0f
	vdup.u8 \mask_vec, \tmp2
	mov \tmp2, #3
	vdup.u8 \mask2_vec, \tmp2
    vec_scalar_mul \tmp_vec5, \tmp_vec0, \tmp_vec1, \mask_vec, \mask2_vec, \tmp_vec2, \tmp_vec3, \tmp_vec4

    // ^ ((in[i] >> 2) & lsb_ask) * ((tab >> 16) & 0xf)
    vldrb.u8 \tmp_vec0, [\in]
    vshr.u8 \tmp_vec0, \tmp_vec0, #2
    mov \tmp2, #1
    vdup.u8 \tmp_vec1, \tmp2   // load lsb_ask   
    vand \tmp_vec0, \tmp_vec0, \tmp_vec1
    lsr \tmp1, \a, #16
    and \tmp1, \tmp1, #0xf

    vdup.u8 \tmp_vec1, \tmp1 // scalar mask
    mov \tmp2, #0x0f
	vdup.u8 \mask_vec, \tmp2
	mov \tmp2, #3
	vdup.u8 \mask2_vec, \tmp2
    vec_scalar_mul \tmp_vec5, \tmp_vec0, \tmp_vec1, \mask_vec, \mask2_vec, \tmp_vec2, \tmp_vec3, \tmp_vec4

    // ^ ((in[i] >> 3) & lsb_ask) * ((tab >> 24) & 0xf);
    vldrb.u8 \tmp_vec0, [\in]
    vshr.u8 \tmp_vec0, \tmp_vec0, #3
    mov \tmp2, #1
    vdup.u8 \tmp_vec1, \tmp2   // load lsb_ask   
    vand \tmp_vec0, \tmp_vec0, \tmp_vec1
    lsr \tmp1, \a, #24
    and \tmp1, \tmp1, #0xf
   
    vdup.u8 \tmp_vec1, \tmp1 // scalar mask
    mov \tmp2, #0x0f
	vdup.u8 \mask_vec, \tmp2
	mov \tmp2, #3
	vdup.u8 \mask2_vec, \tmp2
    vec_scalar_mul \tmp_vec5, \tmp_vec0, \tmp_vec1, \mask_vec, \mask2_vec, \tmp_vec2, \tmp_vec3, \tmp_vec4

    vstrw.u32 \tmp_vec5, [\acc]

    add \in, \in, #16
    add \acc, \acc, #16
    add \tmp0, \tmp0, #1

    cmp \tmp0, #3
    bne 3b
.endm