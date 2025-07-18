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