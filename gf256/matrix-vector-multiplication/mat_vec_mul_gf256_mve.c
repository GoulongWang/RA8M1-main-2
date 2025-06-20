#include <arm_mve.h>
#include <arm_acle.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../randombytes.h"

#define TEST_RUN 100
#define gf256v_madd _gf256v_madd_u32

void gf256mat_prod_ref(uint8_t *c, const uint8_t *matA, unsigned n_A_vec_byte, unsigned n_A_width, const uint8_t *b);
void gf256mat_prod_1936_68(uint8_t *c, const uint8_t *matA, const uint8_t *b);
void gf256mat_prod_68_44(uint8_t *c, const uint8_t *matA, const uint8_t *b);
void gf256mat_prod_44_X(uint8_t *c, const uint8_t *matA, const uint8_t *b, size_t n_A_width);
void unit_test_gf256mat_prod_1936_68();
void unit_test_gf256mat_prod_68_44();
void unit_test_gf256mat_prod_44_X();

int main (void)
{
    unit_test_gf256mat_prod_1936_68();
    unit_test_gf256mat_prod_68_44();
    unit_test_gf256mat_prod_44_X();
    return( 0 );
}

void unit_test_gf256mat_prod_1936_68(){
    printf("====== UOV-Ip: gf256mat_prod 1936_68 unit test ======\n");
    unsigned N_A_VEC_BYTE = 1936, N_A_WIDTH = 68;
    uint8_t matA[ N_A_WIDTH * N_A_VEC_BYTE];
    uint8_t vec_b[ N_A_VEC_BYTE ];
    uint8_t vec_c0[ N_A_VEC_BYTE ];
    uint8_t vec_c1[ N_A_VEC_BYTE ];
    
    int fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes(vec_b, sizeof vec_b);
        randombytes(matA, sizeof matA);

        memset(vec_c0, 0, sizeof(vec_c0));
        memset(vec_c1, 0, sizeof(vec_c1));

        gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b);
        gf256mat_prod_1936_68(vec_c1, matA, vec_b);

        if ( memcmp(vec_c0, vec_c1, N_A_VEC_BYTE) ) {
            printf("test %d:\n", l);
            printf("out_ref = ");
            for (unsigned i = 0; i < N_A_VEC_BYTE; i++) {
                printf("%d ", vec_c0[i]);
            }
            printf("\n\n");
            printf("out_mve = ");
            for (unsigned i = 0; i < N_A_VEC_BYTE; i++) {
                printf("%d ", vec_c1[i]);
            }
            printf("\n");
            fail = 1;
            break;
        }
    }

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
}

void unit_test_gf256mat_prod_68_44(){
    printf("====== UOV-Ip: gf256mat_prod 68_44 unit test ======\n");
    unsigned N_A_VEC_BYTE = 68, N_A_WIDTH = 44;
    uint8_t matA[ N_A_WIDTH * N_A_VEC_BYTE];
    uint8_t vec_b[ N_A_VEC_BYTE ];
    uint8_t vec_c0[ N_A_VEC_BYTE ];
    uint8_t vec_c1[ N_A_VEC_BYTE ];
    
    int fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes(vec_b, sizeof vec_b);
        randombytes(matA, sizeof matA);

        memset(vec_c0, 0, sizeof(vec_c0));
        memset(vec_c1, 0, sizeof(vec_c1));

        gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b);
        gf256mat_prod_68_44(vec_c1, matA, vec_b);

        if ( memcmp(vec_c0, vec_c1, N_A_VEC_BYTE) ) {
            printf("test %d:\n", l);
            printf("out_ref = ");
            for (unsigned i = 0; i < N_A_VEC_BYTE; i++) {
                printf("%d ", vec_c0[i]);
            }
            printf("\n\n");
            printf("out_mve = ");
            for (unsigned i = 0; i < N_A_VEC_BYTE; i++) {
                printf("%d ", vec_c1[i]);
            }
            printf("\n");
            fail = 1;
            break;
        }
    }

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
}

void unit_test_gf256mat_prod_44_X(){
    printf("====== UOV-Ip: gf256mat_prod 44_X unit test ======\n");
    uint8_t N_A_VEC_BYTE = 44, N_A_WIDTH;
    uint8_t vec_b[ N_A_VEC_BYTE ];
    uint8_t vec_c0[ N_A_VEC_BYTE ];
    uint8_t vec_c1[ N_A_VEC_BYTE ];
    
    int fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes(vec_b, sizeof vec_b);
        randombytes(&N_A_WIDTH, sizeof(N_A_WIDTH));
        N_A_WIDTH = N_A_WIDTH % 44 + 1;
        uint8_t matA[ N_A_WIDTH * N_A_VEC_BYTE];
        randombytes(matA, sizeof matA);

        memset(vec_c0, 0, sizeof(vec_c0));
        memset(vec_c1, 0, sizeof(vec_c1));

        gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b);
        gf256mat_prod_44_X(vec_c1, matA, vec_b, N_A_WIDTH);

        if ( memcmp(vec_c0, vec_c1, N_A_VEC_BYTE) ) {
            printf("test %d:\n", l);
            printf("out_ref = ");
            for (unsigned i = 0; i < N_A_VEC_BYTE; i++) {
                printf("%d ", vec_c0[i]);
            }
            printf("\n\n");
            printf("out_mve = ");
            for (unsigned i = 0; i < N_A_VEC_BYTE; i++) {
                printf("%d ", vec_c1[i]);
            }
            printf("\n");
            fail = 1;
            break;
        }
    }

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
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