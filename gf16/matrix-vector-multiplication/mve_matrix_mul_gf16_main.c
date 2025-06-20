#include <stdatomic.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../../randombytes.h"

#define gf256v_add _gf256v_add_u32
#define gf256v_add_mve _gf256v_add_u32_mve
#define gf16v_madd _gf16v_madd_u32

#define N_A_VEC_BYTE  2048 //48 // 2048
#define N_A_WIDTH     96  //64 // 96    
#define TEST_RUN      100

static inline uint32_t gf16v_mul_u32(uint32_t a, uint8_t b) {
    uint32_t a_msb;
    uint32_t a32 = a;
    uint32_t b32 = b;
    uint32_t r32 = a32 * (b32 & 1);

    a_msb = a32 & 0x88888888; // MSB, 3rd bits
    a32 ^= a_msb;   // clear MSB
    a32 = (a32 << 1) ^ ((a_msb >> 3) * 3);
    r32 ^= (a32) * ((b32 >> 1) & 1);

    a_msb = a32 & 0x88888888; // MSB, 3rd bits
    a32 ^= a_msb;   // clear MSB
    a32 = (a32 << 1) ^ ((a_msb >> 3) * 3);
    r32 ^= (a32) * ((b32 >> 2) & 1);

    a_msb = a32 & 0x88888888; // MSB, 3rd bits
    a32 ^= a_msb;   // clear MSB
    a32 = (a32 << 1) ^ ((a_msb >> 3) * 3);
    r32 ^= (a32) * ((b32 >> 3) & 1);

    return r32;

}

static inline void _gf16v_madd_u32_aligned(uint8_t *accu_c, const uint8_t *a, uint8_t gf16_b, unsigned _num_byte) {
    while ( _num_byte >= 4 ) {
        const uint32_t *ax = (const uint32_t *)a;
        uint32_t *cx = (uint32_t *)accu_c;
        cx[0] ^= gf16v_mul_u32( ax[0], gf16_b );
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
    t.u32 = gf16v_mul_u32(t.u32, gf16_b);
    for (unsigned i = 0; i < _num_byte; i++) {
        accu_c[i] ^= t.u8[i];
    }
}

static inline void _gf16v_madd_u32_mve(uint8_t *accu_c, const uint8_t *a, uint8_t gf16_b, unsigned _num_byte) {

    uintptr_t ap = (uintptr_t)(const void *)a;
    uintptr_t cp = (uintptr_t)(const void *)accu_c;
    
    if ( !((cp & 3) || (ap & 3) || (_num_byte < 8)) ) {
        _gf16v_madd_u32_aligned(accu_c, a, gf16_b, _num_byte);
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
        t.u32 = gf16v_mul_u32(t.u32, gf16_b);
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
    t.u32 = gf16v_mul_u32(t.u32, gf16_b);
    for (unsigned i = 0; i < _num_byte; i++) {
        accu_c[i] ^= t.u8[i];
    }
}

static inline void _gf16v_madd_u32(uint8_t *accu_c, const uint8_t *a, uint8_t gf16_b, unsigned _num_byte) {

    uintptr_t ap = (uintptr_t)(const void *)a;
    uintptr_t cp = (uintptr_t)(const void *)accu_c;
    
    if ( !((cp & 3) || (ap & 3) || (_num_byte < 8)) ) {
        _gf16v_madd_u32_aligned(accu_c, a, gf16_b, _num_byte);
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
        t.u32 = gf16v_mul_u32(t.u32, gf16_b);
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
    t.u32 = gf16v_mul_u32(t.u32, gf16_b);
    for (unsigned i = 0; i < _num_byte; i++) {
        accu_c[i] ^= t.u8[i];
    }
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

void _gf256v_add_u32_mve(uint8_t *accu_b, const uint8_t *a, unsigned _num_byte);
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

static inline unsigned gf256v_is_zero(const uint8_t *a, unsigned _num_byte) {
    uint8_t r = 0;
    while ( _num_byte-- ) {
        r |= a[0];
        a++;
    }
    return (0 == r);
}

static inline void gf256v_set_zero(uint8_t *b, unsigned _num_byte) {
    memset(b, 0, _num_byte);
}

static inline uint8_t gf16v_get_ele(const uint8_t *a, unsigned i) {
    uint8_t r = a[i >> 1];
    return (i & 1) ? (r >> 4) : (r & 0xf);
}

void gf16mat_prod_ref(uint8_t *c, const uint8_t *matA, unsigned n_A_vec_byte, unsigned n_A_width, const uint8_t *b) {
    gf256v_set_zero(c, n_A_vec_byte);
    for (unsigned i = 0; i < n_A_width; i++) {
        uint8_t bb = gf16v_get_ele(b, i);
        gf16v_madd(c, matA, bb, n_A_vec_byte);
        matA += n_A_vec_byte;
    }
}

void gf16mat_prod_2048_96(uint8_t *c, const uint8_t *matA, const uint8_t *b);
void gf16mat_prod_48_64(uint8_t *c, const uint8_t *matA, const uint8_t *b);
void gf16mat_prod_32_X(uint8_t *c, const uint8_t *matA, const uint8_t *b, size_t n_A_width);

int main (void)
{
    int fail = 0;
    
    printf("=== UOV-Is: gf16mat_prod 2048_96 Unit Test ===\n");
    uint8_t matA[ N_A_VEC_BYTE * N_A_WIDTH];
    uint8_t vec_b[N_A_VEC_BYTE ];
    uint8_t vec_c0[ N_A_VEC_BYTE ];
    uint8_t vec_c1[ N_A_VEC_BYTE ];
    
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes(matA, sizeof matA);
        randombytes(vec_b, sizeof vec_b);
        memset(vec_c0, 0, sizeof(vec_c0));
        memset(vec_c1, 0, sizeof(vec_c1));
        
        gf16mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b );
        gf16mat_prod_2048_96( vec_c1, matA, vec_b );    
        
        if (memcmp(vec_c0, vec_c1, N_A_VEC_BYTE)) {
            fail = 1;
            break;
        }
    }
    
    /* printf("=== UOV-Is: gf16mat_prod 48_64 Unit Test ===\n");
    uint8_t matA2[ N_A_VEC_BYTE * N_A_WIDTH];
    uint8_t vec_B[N_A_VEC_BYTE ];
    uint8_t vec_C0[ N_A_VEC_BYTE ];
    uint8_t vec_C1[ N_A_VEC_BYTE ];
    
    fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes(matA2, sizeof matA2);
        randombytes(vec_B, sizeof vec_B);
        memset(vec_C0, 0, sizeof(vec_C0));
        memset(vec_C1, 0, sizeof(vec_C1));
        
        gf16mat_prod_ref( vec_C0, matA2, N_A_VEC_BYTE, N_A_WIDTH, vec_B );
        gf16mat_prod_48_64( vec_C1, matA2, vec_B );    
        
        if (memcmp(vec_C0, vec_C1, N_A_VEC_BYTE)) {
            fail = 1;
            break;
        }
    } */ 
  
    /* printf("=== UOV-Is: gf16mat_prod 32_X Unit Test ===\n");
    uint8_t N_A_VEC_BYTE_test = 32, N_A_WIDTH_test; 
    uint8_t out_ref[ N_A_VEC_BYTE_test ];
    uint8_t out_mve[ N_A_VEC_BYTE_test ];

    fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes((uint8_t*) &N_A_WIDTH_test, sizeof(uint8_t));
        N_A_WIDTH_test = N_A_WIDTH_test % 32 + 1;
        uint8_t matA3[ N_A_VEC_BYTE_test * N_A_WIDTH_test ];
        uint8_t vec_B3[N_A_WIDTH_test / 2]; 
        randombytes(matA3, sizeof matA3);
        randombytes(vec_B3, sizeof vec_B3);
        memset(out_ref, 0, sizeof(out_ref));
        memset(out_mve, 0, sizeof(out_mve));
        
        gf16mat_prod_ref( out_ref, matA3, N_A_VEC_BYTE_test, N_A_WIDTH_test, vec_B3);
        gf16mat_prod_32_X(out_mve, matA3, vec_B3, N_A_WIDTH_test);
        
        if (memcmp(out_ref, out_mve, N_A_VEC_BYTE_test)) {
            printf("out_ref = ");
            for (int i = 0; i < 16; i++) {
                printf("%02x ", out_ref[i]);
            }
            printf("\n");

            printf("out_mve = ");
            for (int i = 0; i < 16; i++) {
                printf("%02x ", out_mve[i]);
            }
            printf("\n");
            fail = 1;
            break;
        }
    } */

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n"); 
    return( 0 );
}