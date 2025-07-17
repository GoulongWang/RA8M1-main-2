/* // inout = input/output inplace
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
.endm */