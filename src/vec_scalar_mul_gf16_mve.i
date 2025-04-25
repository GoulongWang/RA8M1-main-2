// inout = input/output inplace
.macro reduce inout, mask1_vec, mask2_vec, tmp_vec0, tmp0
	// lownib(input)
	vand \tmp_vec0, \inout, \mask1_vec
	// highnib(input)
	vshr.u8 \inout, \inout, #4 
	// highnib(input) * 3  
    vmullb.p8 \inout, \inout, \mask2_vec 
	// (highnib(input) * 3) XOR lownib(input)
	veor.u8 \inout, \inout, \tmp_vec0
.endm

.macro sub_vec_scalar_mul output_vec, input_vec, scalar, mask1_vec, mask2_vec, tmp0, tmp_vec0, tmp_vec2, tmp_vec3, shift
	.if \shift == 1
	vshr.u8 \input_vec, \input_vec, #4
	.endif

    vand \tmp_vec0, \input_vec, \mask1_vec
    vdup.u8 \output_vec, \scalar // mask: 用 output_vec 來節省空間
	
    // Multiply bottom half nibbles = [01, xx, 05, xx, 09, xx, 0d, xx] * scalar
	vmullb.p8 \tmp_vec2, \tmp_vec0, \output_vec
	// Multiply top half nibbles = [xx, 03, xx, 07, xx, 0b, xx, 0f] * scalar
	vmullt.p8 \tmp_vec3, \tmp_vec0, \output_vec 

	// output_vec 這時候還不會使用到，可以拿來當暫存向量
	reduce \tmp_vec2, \mask1_vec, \mask2_vec, \output_vec, \tmp0
	reduce \tmp_vec3,  \mask1_vec, \mask2_vec, \output_vec, \tmp0

	// Combine the result of top and bottom
	vshl.u16 \tmp_vec3, \tmp_vec3, #8			  
    veor.u8 \output_vec, \tmp_vec3, \tmp_vec2      
.endm

// 把 output 變成累加器 acc
.macro vec_scalar_mul acc, input_vec, scalar, tmp0, mask_vec, mask2_vec, tmp_vec0, tmp_vec1, tmp_vec2, tmp_vec3
	
	// (1) Compute the result for the least significant 4 bits of each byte
    sub_vec_scalar_mul \tmp_vec3, \input_vec, \scalar, \mask_vec, \mask2_vec, \tmp0, \tmp_vec0, \tmp_vec1, \tmp_vec2, 0
	veor.u8 \acc, \acc, \tmp_vec3

	// (2) Compute the result for the most significant 4 bits of each byte
    sub_vec_scalar_mul \tmp_vec3, \input_vec, \scalar, \mask_vec, \mask2_vec, \tmp0, \tmp_vec0, \tmp_vec1, \tmp_vec2, 1

	// Combine the result of (1) and (2)
	vshl.u8 \tmp_vec3, \tmp_vec3, #4
	veor.u8 \acc, \acc, \tmp_vec3
.endm