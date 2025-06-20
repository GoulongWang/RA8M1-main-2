.syntax unified

#include "mul_add_mat_x_m_mat.i"

.global mul_table_mve
.type mul_table_mve, %function
mul_table_mve:
    push {r8-r11}
    mul_table_mve r0, r12
    pop {r8-r11}
    bx lr

.global m_vec_mul_add_mve
.type m_vec_mul_add_mve, %function
m_vec_mul_add_mve:
    push {r4-r10}

	mov r4, #0x0f
	vdup.u8 q0, r4

	mov r4, #3
	vdup.u8 q1, r4

    m_vec_mul_add_mve r1, r2, r3, r4, r5, r6, q0, q1, q2, q3, q4, q5, q6, q7
    pop {r4-r10}
    bx lr

.global mul_add_mat_x_m_mat_mve
.type mul_add_mat_x_m_mat_mve, %function
mul_add_mat_x_m_mat_mve:
    push {r4-r12, r14}
    
    @ r0 = const int m_vec_limbs = 6
    @ r1 = const unsigned char *mat
    @ r2 = const uint64_t *bs_mat
    @ r3 = uint64_t *acc , acc[MAT_ROWS * BS_MAT_COLS * (M_VEC_LIMBS + 1)];
    @ r4 = const int mat_rows = 10 
    @ r5 = const int mat_cols = 78 
    @ r6 = const int bs_mat_cols = 8 

    ldr r4, [sp, #40]
    ldr r5, [sp, #44]
    ldr r6, [sp, #48]

    mov r7, #0
0:
    mov r8, #0
    1:
        mov r9, #0
        2:
            mov r4, #48
            // in = bs_mat + m_vec_limbs * (c * bs_mat_cols + k)
            // input is uint64t, 所以不能只乘上 m_vec_limbs，因為它的單位不是bytes 要再乘上 8，這樣才會是 64 bytes 
            mul r10, r8, r6
            add r10, r10, r9
            mul r10, r10, r4
            add r10, r10, r2

            // a = mat[r * mat_cols + c]
            mul r11, r7, r5
            add r11, r11, r8
            ldrb r11, [r1, r11] // 要用 ldrb 因為 a 的型態是 char，使用 ldr 會讀取 4 bytes

            // acc = acc + m_vec_limbs * (r * bs_mat_cols + k)
            mul r12, r7, r6
            add r12, r12, r9
            mul r12, r12, r4 // uint64t, 所以不能只乘上 m_vec_limbs，因為它的單位不是bytes 要再乘上 8，這樣才會是 64 bytes 
            add r12, r12, r3

            m_vec_mul_add_mve r10, r11, r12, r14, r4, r0, q0, q1, q2, q3, q4, q5, q6, q7
            
            add r9, r9, #1
            cmp r9, #8
            bne 2b

        add r8, r8, #1
        cmp r8, #78
        bne 1b

    add r7, r7, #1
    cmp r7, #10
    bne 0b

    pop {r4-r12, r14}
    bx lr