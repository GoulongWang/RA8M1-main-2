// inout = input/output inplace
.macro reduce inout, mask1_vec, mask2_vec, tmp_vec0
	vand \tmp_vec0, \inout, \mask1_vec   // lownib(input)
	vshr.u8 \inout, \inout, #4           // highnib(input)
	vmullb.p8 \inout, \inout, \mask2_vec // highnib(input) * 3  
	veor.u8 \inout, \inout, \tmp_vec0    // (highnib(input) * 3) XOR lownib(input)
.endm

.macro sub_vec_scalar_mul output_vec, input_vec, scalar_vec, mask1_vec, mask2_vec, tmp_vec0
    // Multiply bottom half nibbles = [01, xx, 05, xx, 09, xx, 0d, xx] * scalar
	vmullb.p8 \tmp_vec0, \input_vec, \scalar_vec
	// Multiply top half nibbles = [xx, 03, xx, 07, xx, 0b, xx, 0f] * scalar
	vmullt.p8 \input_vec, \input_vec, \scalar_vec

	// output_vec 這時候還不會使用到，可以拿來當暫存向量
	reduce \tmp_vec0, \mask1_vec, \mask2_vec, \output_vec
	reduce \input_vec,  \mask1_vec, \mask2_vec, \output_vec

	// Combine the result of top and bottom
	vshl.u16 \input_vec, \input_vec, #8			  
    veor.u8 \output_vec, \input_vec, \tmp_vec0   
.endm

// 把 output 變成累加器 acc
.macro vec_scalar_mul acc, input_vec, scalar_mask, mask_vec, mask2_vec, tmp_vec0, tmp_vec1, tmp_vec2
	// (1) Compute the result for the least significant 4 bits of each byte
	vand \tmp_vec0, \input_vec, \mask_vec 
	sub_vec_scalar_mul \tmp_vec2, \tmp_vec0, \scalar_mask, \mask_vec, \mask2_vec, \tmp_vec1
	veor.u8 \acc, \acc, \tmp_vec2

	// (2) Compute the result for the most significant 4 bits of each byte
	vshr.u8 \tmp_vec0, \input_vec, #4 
	sub_vec_scalar_mul \tmp_vec2, \tmp_vec0, \scalar_mask, \mask_vec, \mask2_vec, \tmp_vec1

	// Combine the result of (1) and (2), acc = acc "OR" (tmp_vec2 << 4) 這裡 or 的效果等於 xor
	vsli.u8 \acc, \tmp_vec2, #4  
.endm