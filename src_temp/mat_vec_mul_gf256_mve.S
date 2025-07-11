.syntax unified

#include "mat_vec_mul_gf256_mve.i"

.global gf256mat_prod_1936_68
.type gf256mat_prod_1936_68, %function
gf256mat_prod_1936_68:
    gf256mat_prod 1936, 68

.global gf256mat_prod_68_44
.type gf256mat_prod_68_44, %function
gf256mat_prod_68_44:
    gf256mat_prod 68, 44

.global gf256mat_prod_44_X
.type gf256mat_prod_44_X, %function
gf256mat_prod_44_X:
    gf256mat_prod_44_X 44

// 
.global batch_2trimat_madd_gf256_mve
.type batch_2trimat_madd_gf256_mve, %function
batch_2trimat_madd_gf256_mve:
    // r0 = *bC
    // r1 = *btriA
    // r2 = *B
    push {r4-r12, lr}
    sub sp, sp, #3036
    mov r3, sp         @ tmp_c[44] Start Addr
    add r4, r3, #44    @ tmp_Arow[68 * 44] Start Addr
    mov r8, #44
    vmov.u8 q0, #0
    vmov.u8 q1, #0x1b // mask2

    mov r5, #0
7:
    mla r6, r5, r8, r1

    cbz r5, 2f // if i = 0, skip loop
    mov r7, 0
    1:
        mla r10, r7, r8, r4
        memcpy_mve r10, r6, r8, r9, q2
        rsb r9, r7, #66
        mla r6, r9, r8, r6 

        add r7, r7, #1
        cmp r7, r5
        bne 1b
    2:

    mul r9, r5, r8
    add r9, r9, r4
    mov r11, r9 // for memcpy
    mov r10, #44
    memset_mve r9, r10, q0
    add r6, r6, #44

    add r11, r11, #44
    rsb r10, r5, #67
    mul r10, r10, r8
    memcpy_mve r11, r6, r10, r9, q2

    mov r12, #44
    mov r9, r2 
    5: 
        gf256mat_prod_test r3, r4, r9, r8, #68, r6, r7, r10, r11, r14, q1, q2, q3, q4
        sub r4, r4, #2992 // Recover tmpA_row pointer
        gf256v_add_mve r0, r3, r10, q2, q3
        subs r12, r12, #1
        bne 5b

    add r5, r5, #1
    cmp r5, #68
    bne 7b

    add sp, sp, #3036
    pop {r4-r12, lr}
    bx lr