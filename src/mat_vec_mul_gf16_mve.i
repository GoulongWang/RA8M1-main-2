#include "vec_scalar_mul_gf16_mve.i"

.macro gf256v_set_zero_mve c, n_A_vec_byte, tmp0, tmp_vec0
    vmov.u8 \tmp_vec0, #0

    mov \tmp0, \n_A_vec_byte
    lsr \tmp0, \tmp0, #4
7:
    vstrb.u8 \tmp_vec0, [\c], #16
    subs \tmp0, \tmp0, #1
    bne 7b

    sub \c, \c, \n_A_vec_byte
.endm

.macro gf16v_get_ele_mve out, vec_ptr,index, tmp0
    lsrs \tmp0, \index, #1
    ldrb \out, [\vec_ptr, \tmp0]
    ands \tmp0, \index, #1
    ite eq
    andeq \out, \out, #0x0f // == 0 => 取下半 nibble
    lsrne \out, \out, #4    // == 1 => 取上半 nibble
.endm

.macro gf16v_madd_mve c, matA, n_A_vec_byte, tmp0, bb_vector, mask1_vec, mask2_vec, tmp_vec0, tmp_vec1, tmp_vec2, tmp_vec3, tmp_vec4
    mov \tmp0, \n_A_vec_byte
    lsr \tmp0, \tmp0, #4
8:
    vldrb.u8 \tmp_vec0, [\matA], #16 
    vmov.u8 \tmp_vec4, #0 // 把 acc 歸零
    vec_scalar_mul \tmp_vec4, \tmp_vec0, \bb_vector, \mask1_vec, \mask2_vec, \tmp_vec1, \tmp_vec2, \tmp_vec3
    vldrb.u8 \tmp_vec0, [\c]
    veor.u8 \tmp_vec4, \tmp_vec4, \tmp_vec0
    vstrb.u8 \tmp_vec4, [\c], #16
    subs \tmp0, \tmp0, #1
    bne 8b
.endm

.macro gf16mat_prod n_A_vec_byte, n_A_width
    push {r4, r5} 
    @ r0 = c
    @ r1 = matA
    @ r2 = b
     
    gf256v_set_zero_mve r0, \n_A_vec_byte, r3, q0
    vmov.u8 q1, #0x0f // mask1_vec
    vmov.u8 q2, #3    // mask2_vec
    
    mov r3,  #0
0:
    gf16v_get_ele_mve r4, r2, r3, r5
    vdup.u8 q0, r4
    gf16v_madd_mve r0, r1, \n_A_vec_byte, r5, q0, q1, q2, q3, q4, q5, q6, q7

    sub r0, r0, \n_A_vec_byte
    add r3, r3, #1

    cmp r3, \n_A_width
    bne 0b
    
    pop {r4, r5}
    bx lr
.endm

.macro gf16mat_prod_32_X n_A_vec_byte
    push {r4-r6} 
    @ r0 = c
    @ r1 = matA
    @ r2 = b
    @ r3 = n_A_width

    gf256v_set_zero_mve r0, \n_A_vec_byte, r4, q0
    vmov.u8 q1, #0x0f // mask1_vec
    vmov.u8 q2, #3    // mask2_vec
    
    mov r4,  #0
0:
    gf16v_get_ele_mve r6, r2, r4, r5
    vdup.u8 q0, r6
    gf16v_madd_mve r0, r1, \n_A_vec_byte, r5, q0, q1, q2, q3, q4, q5, q6, q7

    sub r0, r0, \n_A_vec_byte
    add r4, r4, #1

    cmp r4, r3
    bne 0b
    
    pop {r4-r6}
    bx lr
.endm

//

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

// ov_publicmap
// inout = input/output inplace
.macro reduce_le_16bytes inout, mask1_vec, mask2_vec, tmp_vec0, size
	vctp.8 \size
    vpstttt
	vandt \tmp_vec0, \inout, \mask1_vec   // lownib(input)
	vshrt.u8 \inout, \inout, #4           // highnib(input)
	vmullbt.p8 \inout, \inout, \mask2_vec // highnib(input) * 3  
	veort.u8 \inout, \inout, \tmp_vec0    // (highnib(input) * 3) XOR lownib(input)
.endm

.macro sub_vec_scalar_mul_le_16bytes output_vec, input_vec, scalar_vec,mask1_vec, mask2_vec, tmp_vec0, size
	vctp.8 \size
	vpstt
    // Multiply bottom half nibbles = [01, xx, 05, xx, 09, xx, 0d, xx] * scalar
	vmullbt.p8 \tmp_vec0, \input_vec, \scalar_vec
	// Multiply top half nibbles = [xx, 03, xx, 07, xx, 0b, xx, 0f] * scalar
	vmulltt.p8 \input_vec, \input_vec, \scalar_vec

	// output_vec 這時候還不會使用到，可以拿來當暫存向量
	reduce \tmp_vec0, \mask1_vec, \mask2_vec, \output_vec
	reduce \input_vec, \mask1_vec, \mask2_vec, \output_vec

	// Combine the result of top and bottom
	vctp.8 \size
	vpstt
	vshlt.u16 \input_vec, \input_vec, #8			  
    veort.u8 \output_vec, \input_vec, \tmp_vec0   
.endm

.macro vec_scalar_mul_le_16bytes acc, input_vec, scalar_mask, mask_vec, mask2_vec, tmp_vec0, tmp_vec1, tmp_vec2, size
    // (1) Compute the result for the least significant 4 bits of each byte
	vctp.8 \size
	vpst
	vandt \tmp_vec0, \input_vec, \mask_vec 
	sub_vec_scalar_mul \tmp_vec2, \tmp_vec0, \scalar_mask, \mask_vec, \mask2_vec, \tmp_vec1
	vctp.8 \size
	vpst
    veort.u8 \acc, \acc, \tmp_vec2

	// (2) Compute the result for the most significant 4 bits of each byte
	vshr.u8 \tmp_vec0, \input_vec, #4 
	sub_vec_scalar_mul \tmp_vec2, \tmp_vec0, \scalar_mask, \mask_vec, \mask2_vec, \tmp_vec1

	// Combine the result of (1) and (2), acc = acc "OR" (tmp_vec2 << 4) 這裡 or 的效果等於 xor
    vsli.u8 \acc, \tmp_vec2, #4  
.endm

.macro gf16_vector_scalar inout_ptr, gf16_b, _num_byte, size, input_vec, scalar_mask, mask_vec, mask2_vec, tmp_vec0, tmp_vec1, tmp_vec2, acc
    vdup.u8 \scalar_mask, \gf16_b
4:
    // size = (_num_byte >= 16)? 16: _num_byte
    mov \size, #16
    cmp \_num_byte, #16
    it lt // less than
    movlt \size, \_num_byte  
    vmov.u8 \acc, #0
    vldrb.u8 \input_vec, [\inout_ptr]
    vec_scalar_mul_le_16bytes \acc, \input_vec, \scalar_mask, \mask_vec, \mask2_vec, \tmp_vec0, \tmp_vec1, \tmp_vec2, \size
    vstrb.u8 \acc, [\inout_ptr]
    
    add \inout_ptr, \inout_ptr, \size
    subs \_num_byte, \_num_byte, #16
    bgt 4b
.endm