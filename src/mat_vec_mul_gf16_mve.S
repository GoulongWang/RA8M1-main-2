.syntax unified

#include "mat_vec_mul_gf16_mve.i"

.global gf16mat_prod_mve
.type gf16mat_prod_mve, %function
gf16mat_prod_mve:
    push {r4, r5} 
    @ r0 = c
    @ r1 = matA
    @ r2 = n_A_vec_byte
    @ r3 = n_A_width
    @ r4 = b
    ldr r4, [sp, #8] 
     
    gf256v_set_zero_mve r0, r5, q0
    
    mov r5,  #0
0:
    gf16v_get_ele_mve r6, r4, r5, r7
    gf16v_madd_mve r0, r1, r6, r7, q0, q1, q2, q3, q4, q5, q6, q7

    sub r0, r0, #2048 // c
    add r5, r5, #1

    // low-overhead branches
    cmp r5, #96
    bne 0b
1:
    pop {r4, r5}
    bx lr