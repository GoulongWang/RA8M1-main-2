#ifndef UNIT_TEST

#ifdef RA8M1
#include "hal_data.h"
#elif defined(QEMU_M55)
#include "SSE300MPS3.h"
#endif

#include <arm_mve.h>
#include <arm_acle.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "randombytes.h"
#include "utils.h"

#define gf256v_add _gf256v_add_u32
#define gf256v_madd _gf256v_madd_u32

#define N_A_VEC_BYTE 1936 //68//1936
#define N_A_WIDTH    68   //44//68
#define TEST_RUN 100

#define bench_cycles(CALL, OUT_VAR)                                                    \
    do {                                                               \
        __disable_irq();                                               \
                                                                       \
        ARM_PMU_CYCCNT_Reset();                                        \
                                                                       \
        ARM_PMU_CNTR_Enable(PMU_CNTENSET_CCNTR_ENABLE_Msk);            \
        CALL;                                                          \
        ARM_PMU_CNTR_Disable(PMU_CNTENCLR_CCNTR_ENABLE_Msk);           \
                                                                       \
        printf(#CALL ": cycles = %" PRIu32 "\n", ARM_PMU_Get_CCNTR()); \
        OUT_VAR = ARM_PMU_Get_CCNTR();                                 \
                                                                       \
        __enable_irq();                                                \
    }                                                                  \
    while (0)

static inline unsigned gf256v_is_zero(const uint8_t *a, unsigned _num_byte) {
    uint8_t r = 0;
    while ( _num_byte-- ) {
        r |= a[0];
        a++;
    }
    return (0 == r);
}

static inline void _gf256v_add_u32_aligned(uint8_t *accu_b, const uint8_t *a, unsigned _num_byte) {
    while ( _num_byte >= 4 ) {
        uint32_t *bx = (uint32_t *)accu_b;
        uint32_t *ax = (uint32_t *)a;
        bx[0] ^= ax[0];
        a += 4;
        accu_b += 4;
        _num_byte -= 4;
    }
    while ( _num_byte ) {
        _num_byte--;
        accu_b[_num_byte] ^= a[_num_byte];
    }
}

static inline void _gf256v_add_u32(uint8_t *accu_b, const uint8_t *a, unsigned _num_byte) {
    uintptr_t bp = (uintptr_t)(const void *)accu_b;
    uintptr_t ap = (uintptr_t)(const void *)a;
    if ( !((bp & 3) || (ap & 3) || (_num_byte < 8)) ) {
        _gf256v_add_u32_aligned(accu_b, a, _num_byte);
        return;
    }

    while ( _num_byte ) {
        _num_byte--;
        accu_b[_num_byte] ^= a[_num_byte];
    }
}


static inline uint32_t gf256v_mul_u32(uint32_t a, uint8_t b) {
    uint32_t a_msb;
    uint32_t a32 = a;
    uint32_t b32 = b;
    uint32_t r32 = a32 * (b32 & 1);

    a_msb = a32 & 0x80808080; // MSB, 7th bits
    a32 ^= a_msb;   // clear MSB
    a32 = (a32 << 1) ^ ((a_msb >> 7) * 0x1b);
    r32 ^= (a32) * ((b32 >> 1) & 1);

    a_msb = a32 & 0x80808080; // MSB, 7th bits
    a32 ^= a_msb;   // clear MSB
    a32 = (a32 << 1) ^ ((a_msb >> 7) * 0x1b);
    r32 ^= (a32) * ((b32 >> 2) & 1);

    a_msb = a32 & 0x80808080; // MSB, 7th bits
    a32 ^= a_msb;   // clear MSB
    a32 = (a32 << 1) ^ ((a_msb >> 7) * 0x1b);
    r32 ^= (a32) * ((b32 >> 3) & 1);

    a_msb = a32 & 0x80808080; // MSB, 7th bits
    a32 ^= a_msb;   // clear MSB
    a32 = (a32 << 1) ^ ((a_msb >> 7) * 0x1b);
    r32 ^= (a32) * ((b32 >> 4) & 1);

    a_msb = a32 & 0x80808080; // MSB, 7th bits
    a32 ^= a_msb;   // clear MSB
    a32 = (a32 << 1) ^ ((a_msb >> 7) * 0x1b);
    r32 ^= (a32) * ((b32 >> 5) & 1);

    a_msb = a32 & 0x80808080; // MSB, 7th bits
    a32 ^= a_msb;   // clear MSB
    a32 = (a32 << 1) ^ ((a_msb >> 7) * 0x1b);
    r32 ^= (a32) * ((b32 >> 6) & 1);

    a_msb = a32 & 0x80808080; // MSB, 7th bits
    a32 ^= a_msb;   // clear MSB
    a32 = (a32 << 1) ^ ((a_msb >> 7) * 0x1b);
    r32 ^= (a32) * ((b32 >> 7) & 1);

    return r32;
}

static inline void gf256v_set_zero(uint8_t *b, unsigned _num_byte) {
    memset(b, 0, _num_byte);
}

static inline void _gf256v_madd_u32_aligned(uint8_t *accu_c, const uint8_t *a, uint8_t gf256_b, unsigned _num_byte) {
    while ( _num_byte >= 4 ) {
        const uint32_t *ax = (const uint32_t *)a;
        uint32_t *cx = (uint32_t *)accu_c;
        cx[0] ^= gf256v_mul_u32( ax[0], gf256_b );
        a += 4;
        accu_c += 4;
        _num_byte -= 4;
    }

    if ( 0 == _num_byte ) {
        return;
    }

    union tmp_32 {
        uint8_t u8[4];
        uint32_t u32;
    } t;
    for (unsigned i = 0; i < _num_byte; i++) {
        t.u8[i] = a[i];
    }
    #if defined(_VALGRIND_)
    VALGRIND_MAKE_MEM_DEFINED(&t.u32, 4);
    #endif
    t.u32 = gf256v_mul_u32(t.u32, gf256_b);
    for (unsigned i = 0; i < _num_byte; i++) {
        accu_c[i] ^= t.u8[i];
    }
}

static inline void _gf256v_madd_u32(uint8_t *accu_c, const uint8_t *a, uint8_t gf256_b, unsigned _num_byte) {

    uintptr_t ap = (uintptr_t)(const void *)a;
    uintptr_t cp = (uintptr_t)(const void *)accu_c;
    if ( !((cp & 3) || (ap & 3) || (_num_byte < 8)) ) {
        _gf256v_madd_u32_aligned(accu_c, a, gf256_b, _num_byte);
        return;
    }

    union tmp_32 {
        uint8_t u8[4];
        uint32_t u32;
    } t;

    while ( _num_byte >= 4 ) {
        t.u8[0] = a[0];
        t.u8[1] = a[1];
        t.u8[2] = a[2];
        t.u8[3] = a[3];
        t.u32 = gf256v_mul_u32(t.u32, gf256_b);
        accu_c[0] ^= t.u8[0];
        accu_c[1] ^= t.u8[1];
        accu_c[2] ^= t.u8[2];
        accu_c[3] ^= t.u8[3];
        a += 4;
        accu_c += 4;
        _num_byte -= 4;
    }
    if ( 0 == _num_byte ) {
        return;
    }

    for (unsigned i = 0; i < _num_byte; i++) {
        t.u8[i] = a[i];
    }
    #if defined(_VALGRIND_)
    VALGRIND_MAKE_MEM_DEFINED(&t.u32, 4);
    #endif
    t.u32 = gf256v_mul_u32(t.u32, gf256_b);
    for (unsigned i = 0; i < _num_byte; i++) {
        accu_c[i] ^= t.u8[i];
    }

}

void gf256mat_prod_ref(uint8_t *c, const uint8_t *matA, unsigned n_A_vec_byte, unsigned n_A_width, const uint8_t *b) {
    gf256v_set_zero(c, n_A_vec_byte);
    for (unsigned i = 0; i < n_A_width; i++) {
        gf256v_madd(c, matA, b[i], n_A_vec_byte);
        matA += n_A_vec_byte;
    }
}

void gf256mat_prod_1936_68(uint8_t *c, const uint8_t *matA, const uint8_t *b);
void gf256mat_prod_68_44(uint8_t *c, const uint8_t *matA, const uint8_t *b);
void gf256mat_prod_44_X(uint8_t *c, const uint8_t *matA, const uint8_t *b, size_t n_A_width);

void gf256mat_prod_m4f_1936_68_normal_normal(uint32_t *c, uint32_t *a, uint8_t *b);
void gf256mat_prod_m4f_68_44_normal_normal(uint32_t *c, uint32_t *a, uint8_t *b);
void gf256mat_prod_m4f_44_X_normal_normal(uint32_t *c, uint32_t *a, uint8_t *b, size_t n_A_width);

void benchmark_gf256mat_prod_1936_68(){
    uint32_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint32_t cycles;
    int fail = 0;

    printf("====== UOV-Ip: gf256mat_prod 1936_68 unit test ======\n");
    uint8_t matA[ N_A_WIDTH * N_A_VEC_BYTE];
    uint8_t vec_b[ N_A_VEC_BYTE ];
    uint8_t vec_c0[ N_A_VEC_BYTE ];
    uint8_t vec_c1[ N_A_VEC_BYTE ];
    uint8_t vec_c2[ N_A_VEC_BYTE ];
    
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes(vec_b, sizeof vec_b);
        randombytes(matA, sizeof matA);

        memset(vec_c0, 0, sizeof(vec_c0));
        memset(vec_c1, 0, sizeof(vec_c1));
        memset(vec_c2, 0, sizeof(vec_c2));

        bench_cycles(gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b), cycles);
        sum_ref += cycles;
        bench_cycles(gf256mat_prod_1936_68(vec_c1, matA, vec_b), cycles);
        sum_mve += cycles;
        bench_cycles(gf256mat_prod_m4f_1936_68_normal_normal((uint32_t *)vec_c2, (uint32_t *)matA, (uint8_t *)vec_b), cycles);
        sum_m4 += cycles;

        gf256v_add( vec_c0, vec_c1, N_A_VEC_BYTE );
        if ( !gf256v_is_zero( vec_c0, N_A_VEC_BYTE ) ) {
            gf256v_add( vec_c0, vec_c1, N_A_VEC_BYTE );
            printf("test %d:\n", l);
            printf("out_ref = ");
            for (int i = 0; i < N_A_VEC_BYTE; i++) {
                printf("%d ", vec_c0[i]);
            }
            printf("\n\n");
            printf("out_mve = ");
            for (int i = 0; i < N_A_VEC_BYTE; i++) {
                printf("%d ", vec_c1[i]);
            }
            printf("\n");
            fail = 1;
            break;
        }
    }

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
    printf("Average ref cycles = %lu\n", sum_ref / TEST_RUN);
    printf("Average MVE cycles = %lu\n", sum_mve / TEST_RUN);
    printf("Average M4 cycles = %lu\n", sum_m4 / TEST_RUN);
}

ITCM_FN int main (void)
{
    Utils_Init();
    PMU_Init();
    benchmark_gf256mat_prod_1936_68();

    /*
    printf("====== UOV-Ip: gf256mat_prod 68_44 unit test ======\n");
    uint8_t matA[ N_A_WIDTH * N_A_VEC_BYTE];
    uint8_t vec_b[ N_A_VEC_BYTE ];
    uint8_t vec_c0[ N_A_VEC_BYTE ];
    uint8_t vec_c1[ N_A_VEC_BYTE ];
    uint8_t vec_c2[ N_A_VEC_BYTE ];
    
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes(vec_b, sizeof vec_b);
        randombytes(matA, sizeof matA);

        memset(vec_c0, 0, sizeof(vec_c0));
        memset(vec_c1, 0, sizeof(vec_c1));
        memset(vec_c2, 0, sizeof(vec_c2));

        bench_cycles(gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b), cycles);
        sum_ref += cycles;
        bench_cycles(gf256mat_prod_68_44(vec_c1, matA, vec_b), cycles);
        sum_mve += cycles;
        bench_cycles(gf256mat_prod_m4f_68_44_normal_normal((uint32_t *)vec_c2, (uint32_t *)matA, vec_b), cycles);
        sum_m4 += cycles;

        gf256v_add( vec_c0, vec_c1, N_A_VEC_BYTE );

        if ( !gf256v_is_zero( vec_c0, N_A_VEC_BYTE ) ) {
            gf256v_add( vec_c0, vec_c1, N_A_VEC_BYTE );
            printf("test %d:\n", l);
            printf("out_ref = ");
            for (int i = 0; i < N_A_VEC_BYTE; i++) {
                printf("%d ", vec_c0[i]);
            }
            printf("\n\n");
            printf("out_mve = ");
            for (int i = 0; i < N_A_VEC_BYTE; i++) {
                printf("%d ", vec_c1[i]);
            }
            printf("\n");
            fail = 1;
            break;
        }
    } */
/*
    
    printf("====== UOV-Ip: gf256mat_prod 44_X unit test ======\n");
    uint32_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint32_t cycles;
    uint8_t N_A_VEC_BYTE_test = 44, N_A_WIDTH_test;
    uint8_t vec_b[ N_A_VEC_BYTE_test ];
    uint8_t vec_c0[ N_A_VEC_BYTE_test ];
    uint8_t vec_c1[ N_A_VEC_BYTE_test ];
    uint8_t vec_c2[ N_A_VEC_BYTE_test ];
    
    int fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes(vec_b, sizeof vec_b);
        randombytes(&N_A_WIDTH_test, sizeof(N_A_WIDTH_test));
        N_A_WIDTH_test = N_A_WIDTH_test % 44 + 1;
        uint8_t matA[ N_A_WIDTH_test * N_A_VEC_BYTE_test];
        randombytes(matA, sizeof matA);

        memset(vec_c0, 0, sizeof(vec_c0));
        memset(vec_c1, 0, sizeof(vec_c1));
        memset(vec_c2, 0, sizeof(vec_c2));

        bench_cycles(gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE_test, N_A_WIDTH_test, vec_b), cycles);
        sum_ref += cycles;
        bench_cycles(gf256mat_prod_44_X(vec_c1, matA, vec_b, N_A_WIDTH_test), cycles);
        sum_mve += cycles;
        bench_cycles(gf256mat_prod_m4f_44_X_normal_normal((uint32_t *)vec_c2, (uint32_t *)matA, vec_b, N_A_WIDTH_test), cycles);
        sum_m4 += cycles;

        gf256v_add( vec_c0, vec_c1, N_A_VEC_BYTE_test );

        if ( !gf256v_is_zero( vec_c0, N_A_VEC_BYTE_test ) ) {
            gf256v_add( vec_c0, vec_c1, N_A_VEC_BYTE_test );
            printf("test %d:\n", l);
            printf("out_ref = ");
            for (int i = 0; i < N_A_VEC_BYTE_test; i++) {
                printf("%d ", vec_c0[i]);
            }
            printf("\n\n");
            printf("out_mve = ");
            for (int i = 0; i < N_A_VEC_BYTE_test; i++) {
                printf("%d ", vec_c1[i]);
            }
            printf("\n");
            fail = 1;
            break;
        }
    }

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
    printf("Average ref cycles = %lu\n", sum_ref / TEST_RUN);
    printf("Average MVE cycles = %lu\n", sum_mve / TEST_RUN);
    printf("Average M4 cycles = %lu\n", sum_m4 / TEST_RUN);
    */
    return( 0 );
}
#endif