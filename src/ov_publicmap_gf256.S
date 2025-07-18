.syntax unified

#include "mat_vec_mul_gf256_mve.i"

// void ov_publicmap_gf256_mve( unsigned char *y, const unsigned char *trimat, const unsigned char *x );
.global ov_publicmap_gf256_mve
.type ov_publicmap_gf256_mve, %function
ov_publicmap_gf256_mve:
    push {r4-r12, r14}
    sub sp, sp, #512 
    mov r3, sp        // _xixj[256]
    add r4, sp, #256  // _x[256]
    vmov.u8 q3, #0x1b // mask2

    gf256v_set_zero_mve r3, r5, q0, #256
    gf256v_set_zero_mve r0, r5, q0, #44
    //gf256v_set_zero_mve r4, r5, q0, #256

    mov r5, #7
    mov r6, r2 // &x[0]
    mov r7, r4 // &_x[0]
0:  
    vldrb.u8 q0, [r6], #16
    vstrb.u8 q0, [r7], #16
    subs r5, r5, #1
    bne 0b

    // P1
    mov r5, #0
    mov r11, r4 // &_x[0]: the first addr of gf256_b 
8:  
    add r8, r4, r5   // &_x[i]    : start addr of _x[j]
    add r9, r3, r5   // &_xixj[i] : start addr of _xixj[j]
    rsb r6, r5, #68  // num_bytes = 68 - i
    lsr r7, r6, #4   // cnt = num_bytes / 16
    cbz r7, 5f       // if cnt = 0, skip
    9:
        vldrb.u8 q0, [r8], #16
        vstrb.u8 q0, [r9], #16
        subs r7, r7, #1
        bne 9b

    5:
        and r7, r6, #0xF // left_bytes = num_bytes % 16
        cbz r7, 6f
        vctp.8 r7  // 建立 predicate
        vpstt
        vldrbt.u8 q0, [r8]
        vstrbt.u8 q0, [r9]
    6:

    and r6, r5, #3 
    sub r6, r5, r6     // r6 = i_start
    rsb r7, r6, #68    // num_bytes
    ldrb r8, [r11], #1 // gf256b 
    add r9, r6, r3     // inout
    gf256_vector_scalar r9, r8, r7, r10, r12, r14, q0, q1, q2, q3
    sub r9, r9, r7

    rsb r6, r5, #68
    add r7, r3, r5 // &_xixj[i] : start addr of _xixj[j]
    7:
        ldrb r8, [r7], #1 //_xixj[j]
        gf256v_madd_mve r0, r1, r8, #44, r9, r10, r12, q0, q1, q2, q3
        subs r6, r6, #1
        bne 7b 
 
    add r5, r5, #1
    cmp r5, #68
    bne 8b 

  
    // P2
    mov r5, #0
    mov r10, r4 // the first addr of _x[i]
13:  
    add r8, r4, #68  // start addr of _x[v + j]
    mov r9, r3       // start addr of _xixj[j]
    mov r6, #44      // num_bytes
    lsr r7, r6, #4   // cnt = num_bytes / 16
    cbz r7, 14f      // if cnt = 0, skip
    15:
        vldrb.u8 q0, [r8], #16
        vstrb.u8 q0, [r9], #16
        subs r7, r7, #1
        bne 15b

    14:
        and r7, r6, #0xF // left_bytes = num_bytes % 16
        cbz r7, 16f
        vctp.8 r7  // 建立 predicate
        vpstt
        vldrbt.u8 q0, [r8]
        vstrbt.u8 q0, [r9]
    16:

    mov r6, #44
    ldrb r7, [r10], #1
    mov r8, r3
    gf256_vector_scalar r8, r7, r6, r11, r12, r14, q0, q1, q2, q3
    sub r8, r8, #44
    
    mov r6, #44
    mov r7, r3 // start addr of _xixj[j]
    17:
        ldrb r8, [r7], #1 //_xixj[j]
        gf256v_madd_mve r0, r1, r8, #44, r9, r11, r12, q0, q1, q2, q3
        subs r6, r6, #1
        bne 17b 

    add r5, r5, #1
    cmp r5, #68
    bne 13b  

     // P3
    mov r5, #0
    add r10, r4, #68 // the first addr of _x[ v + i]
18:  
    add r8, r4, #68  
    add r8, r8, r5   // start addr of _x[v + j]
    add r9, r3, r5   // start addr of _xixj[j]
    rsb r6, r5, #44  // num_bytes
    lsr r7, r6, #4   // cnt = num_bytes / 16
    cbz r7, 19f      // if cnt = 0, skip
    20:
        vldrb.u8 q0, [r8], #16
        vstrb.u8 q0, [r9], #16
        subs r7, r7, #1
        bne 20b

    19:
        and r7, r6, #0xF // left_bytes = num_bytes % 16
        cbz r7, 21f
        vctp.8 r7  // 建立 predicate
        vpstt
        vldrbt.u8 q0, [r8]
        vstrbt.u8 q0, [r9]
    21: 

    and r6, r5, #3 
    sub r6, r5, r6     // r6 = i_start
    rsb r7, r6, #44
    ldrb r8, [r10], #1
    add r9, r3, r6
    gf256_vector_scalar r9, r8, r7, r11, r12, r14, q0, q1, q2, q3
    sub r9, r9, r7
    
    rsb r6, r5, #44
    add r7, r3, r5 // start addr of _xixj[j]
    22:
        ldrb r8, [r7], #1 //_xixj[j]
        gf256v_madd_mve r0, r1, r8, #44, r9, r11, r12, q0, q1, q2, q3
        subs r6, r6, #1
        bne 22b

    add r5, r5, #1
    cmp r5, #44
    bne 18b  
  
    add sp, sp, #512
    pop {r4-r12, r14}
    bx lr 