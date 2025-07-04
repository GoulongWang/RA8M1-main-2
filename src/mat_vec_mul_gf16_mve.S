.syntax unified

#include "mat_vec_mul_gf16_mve.i"

.global gf16mat_prod_2048_96
.type gf16mat_prod_2048_96, %function
gf16mat_prod_2048_96:
    gf16mat_prod 2048, 96

.global gf16mat_prod_48_64
.type gf16mat_prod_48_64, %function
gf16mat_prod_48_64:
    gf16mat_prod 48, 64

.global gf16mat_prod_32_X
.type gf16mat_prod_32_X, %function
gf16mat_prod_32_X:
    gf16mat_prod_32_X 32

.global batch_2trimat_madd_gf16_mve
.type batch_2trimat_madd_gf16_mve, %function
batch_2trimat_madd_gf16_mve:
    @ r0 = unsigned char *bC, 
    @ r1 = const unsigned char *btriA,
    @ r2 = const unsigned char *B, 

    push {r4-r12, r14}
    vmov.u8 q0, #0x0f // mask1_vec for gf16mat_prod
    vmov.u8 q1, #3    // mask2_vec for gf16mat_prod
    // Allocate the memory for tmp_c and tmp_Arow = 32 + 32 * 96 = 3104 bytes
    sub sp, sp, #3104
    mov r14, sp         @ tmp_c[32] Start Addr
    add r12, r14, #32   @ tmp_Arow[32 * 96] = tmp_Arow[3072] Start Addr
    mov r11, #32
    mov r10, r12
    mov r9, r1

    mov r5, #96
0:
    rsb r7, r5, #96
    // r9 = ptr = btriA + i * size_batch = r1 + r5 << 5
    add r9, r1, r7, lsl #5 

    cbz r7, 2f // if i = 0, skip loop

    mov r6, r12
    mov r3, #0
    1:  
        memcpy_mve r6, r9, r11, r4, q2

        // ptr += (94 - j) * 32 = 3008 - 32j
        add r9, r9, #3008
        sub r9, r9, r3, lsl #5
        
        add r3, r3, #1
        cmp r3, r7
        bne 1b
    2:

    vmov.u8 q2, #0
    vstrb.u8 q2, [r10], #16
    vstrb.u8 q2, [r10], #16

    add r9, r9, #32
 
    mov r8, r10
    rsb r6, r7, #95 // 感覺可優化，但目前暫存器不夠
    lsl r6, r6, #5
    memcpy_mve r8, r9, r6, r4, q2 

    mov r6, r2
    mov r8, #64
    5:
        gf16mat_prod_test r14, r12, r6, #32, #96, r3, r7, r4, q2, q0, q1, q3, q4, q5, q6, q7
        gf256v_add_mve r0, r14, q2, q3
        sub r12, r12, #3072
        add r6, r6, #48
        subs r8, r8, #1
        bne 5b  

    subs r5, r5, #1
    bne 0b

    add sp, sp, #3104
    pop {r4-r12, r14}
    bx lr

// 
// void ov_publicmap_mve( unsigned char *y, const unsigned char *trimat, const unsigned char *x )
.global ov_publicmap_mve
.type ov_publicmap_mve, %function
ov_publicmap_mve:
    push {r4-r12}
    sub sp, sp, #512 
    mov r3, sp        // _xixj[256]
    add r4, sp, #256  // _x[256]
	vmov.u8 q2, #0x0f
	vmov.u8 q3, #3
    
    mov r5, #0
    mov r8, r4 // &x[0]
0:  
    gf16v_get_ele_mve r7, r2, r5, r6
    strb r7, [r8], #1 // _x[i]

    add r5, r5, #1
    cmp r5, #64
    bne 0b
 
    // P1
    mov r5, #0
    mov r11, r4 // &_x[0]: the first addr of gf16_b 
1:  
    and r6, r5, #3 
    sub r6, r5, r6 // r6 = i_start
    
    rsb r7, r5, #32
    add r8, r4, r5 // &_x[i]    : start addr of _x[j]
    add r9, r3, r5 // &_xixj[i] : start addr of _xixj[j]
    2:
        ldrb r10, [r8], #1
        strb r10, [r9], #1
        subs r7, r7, #1
        bne 2b
    
    rsb r7, r6, #32    // num_bytes
    ldrb r8, [r11], #1 // gf16b 
    add r6, r6, r3     // inout
    gf16_vector_scalar r6, r8, r7, r9, q0, q1, q2, q3, q4, q5, q6, q7

    rsb r6, r5, #32
    add r7, r3, r5 // &_xixj[i] : start addr of _xixj[j]
    3:
        ldrb r8, [r7], #1 //_xixj[j]
        vdup.u8 q1, r8
        gf16v_madd_mve r0, r1, #32, r8, q1, q2, q3, q4, q5, q6, q7, q0
        sub r0, r0, #32
        
        subs r6, r6, #1
        bne 3b

    add r5, r5, #1
    cmp r5, #32
    bne 1b

    // P2
    mov r5, #0
    mov r10, r4 // the first addr of _x[i]
5:  
    mov r6, #32
    add r7, r4, #32 // the first addr of _x[v + j]
    mov r8, r3      // the first addr of _xixj[j]
    6:
        ldrb r9, [r7], #1
        strb r9, [r8], #1

        subs r6, r6, #1
        bne 6b
    
    mov r6, #32 
    ldrb r7, [r10], #1
    mov r8, r3
    gf16_vector_scalar r8, r7, r6, r9, q0, q1, q2, q3, q4, q5, q6, q7

    rsb r6, #32
    mov r7, r3
    7:
        ldrb r8, [r7], #1 //_xixj[j]
        vdup.u8 q1, r8    // bb_vector
        gf16v_madd_mve r0, r1, #32, r8, q1, q2, q3, q4, q5, q6, q7, q0
        sub r0, r0, #32
        
        subs r6, r6, #1
        bne 7b

    add r5, r5, #1
    cmp r5, #32
    bne 5b

    // P3
    mov r5, #0
    add r11, r4, #32 // the first addr of _x[v + i] = r4 + 32 + 0
9:  
    and r6, r5, #3 
    sub r6, r5, r6 // r6 = i_start
    
    rsb r7, r5, #32
    add r8, r4, r5 
    add r8, r8, #32 // the first addr of _x[v + j]
    add r9, r3, r5  // the first addr of _xixj[j]
    10:
        ldrb r10, [r8], #1
        strb r10, [r9], #1
        subs r7, r7, #1
        bne 10b

    rsb r7, r6, #32    // num_byte = 32 - i_start
    ldrb r8, [r11], #1 // _x[v + i]
    add r9, r3, r6     // inout = _xixj + i_start
    gf16_vector_scalar r9, r8, r7, r10, q0, q1, q2, q3, q4, q5, q6, q7

    rsb r6, r5, #32
    add r7, r3, r5 // the first addr of _xixj[j]
    11:
        ldrb r8, [r7], #1
        vdup.u8 q0, r8 // bb_vector
        gf16v_madd_mve r0, r1, #32, r8, q0, q2, q3, q4, q5, q6, q7, q1
        sub r0, r0, #32
         
        subs r6, r6, #1
        bne 11b

    add r5, r5, #1
    cmp r5, #32
    bne 9b

    add sp, sp, #512
    pop {r4-r12}
    bx lr 