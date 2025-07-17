#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../randombytes.h"

#define TEST_RUN 100
#define N 112
#define M 44
#define _MAX_N 256
#define TMPVEC_LEN 128
#define _PUB_N 112
#define _PUB_M 44
#define _O (_PUB_M)
#define _V ((_PUB_N)-(_O))
#define _PUB_M_BYTE (_PUB_M )
#define _GFSIZE 256

#define gfv_mul_scalar _gf256v_mul_scalar_u32
#define gf256v_madd    _gf256v_madd_u32
void ov_publicmap( unsigned char *y, const unsigned char *trimat, const unsigned char *x );
void ov_publicmap_gf256_mve( unsigned char *y, const unsigned char *trimat, const unsigned char *x );

int main(){
    printf("=== UOV-Ip: ov_publicmap Unit Test ===\n");
    uint8_t P[M * N * (N + 1) / 2];
    uint8_t sig[N];
    uint8_t acc[M], acc_mve[M];

    int fail = 0;
    for (int i = 0; i < TEST_RUN; i++) {
        memset(acc, 0, sizeof(acc));
        memset(acc_mve, 0, sizeof(acc_mve));
        randombytes(P, sizeof(P));
        randombytes(sig, sizeof sig);

        ov_publicmap(acc, P, sig);
        ov_publicmap_gf256_mve(acc_mve, P, sig);
        
        if(memcmp(acc_mve, acc, sizeof(acc))){
            printf("acc_ref = [");
            for (int i = 0; i < sizeof(acc); i++) {
                printf("%02x ", acc[i]);
            }
            printf("]\n");

            printf("acc_mve = [");
            for (int i = 0; i < sizeof(acc_mve); i++) {
                printf("%02x ", acc_mve[i]);
            }
            printf("]\n"); 
            fail = 1;
            break;
        }  
    } 
    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
    return 0;
}

static inline uint8_t gf256v_get_ele(const uint8_t *a, unsigned i) {
    return a[i];
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

static inline void _gf256v_mul_scalar_u32_aligned(uint8_t *a, uint8_t b, unsigned _num_byte) {

    while ( _num_byte >= 4 ) {
        uint32_t *ax = (uint32_t *)a;
        ax[0] = gf256v_mul_u32( ax[0], b );
        a += 4;
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
    t.u32 = gf256v_mul_u32(t.u32, b);
    for (unsigned i = 0; i < _num_byte; i++) {
        a[i] = t.u8[i];
    }
}

static inline void _gf256v_mul_scalar_u32(uint8_t *a, uint8_t b, unsigned _num_byte) {

    uintptr_t ap = (uintptr_t)(const void *)a;
    if ( !((ap & 3) || (_num_byte < 8)) ) {
        _gf256v_mul_scalar_u32_aligned(a, b, _num_byte);
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
        t.u32 = gf256v_mul_u32(t.u32, b);
        a[0] = t.u8[0];
        a[1] = t.u8[1];
        a[2] = t.u8[2];
        a[3] = t.u8[3];
        a += 4;
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
    t.u32 = gf256v_mul_u32(t.u32, b);
    for (unsigned i = 0; i < _num_byte; i++) {
        a[i] = t.u8[i];
    }

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

void ov_publicmap( unsigned char *y, const unsigned char *trimat, const unsigned char *x ) {
    uint8_t _xixj[_MAX_N] = {0};
    uint8_t _x[_MAX_N];
    unsigned v = _V;
    unsigned o = _PUB_N - _V;
    unsigned vec_len = _PUB_M_BYTE;

    for (unsigned i = 0; i < _PUB_N; i++) {
        _x[i] =  gf256v_get_ele( x, i );
    }
    
    // P1
    for (unsigned i = 0; i < _V; i++) {
        unsigned i_start = i - (i & 3);
        for (unsigned j = i; j < _V; j++) {
            _xixj[j] = _x[j];
        }
        
        gfv_mul_scalar( _xixj + i_start, _x[i], v - i_start );      
        
        for (unsigned j = i; j <  v; j++) {
            gf256v_madd(y, trimat, _xixj[j], vec_len);
            trimat += vec_len;
        }
    }   
       
    // P2
    for (unsigned i = 0; i < v; i++) {
        for (unsigned j = 0; j < o; j++) {
            _xixj[j] = _x[v + j];
        }   
        
        gfv_mul_scalar( _xixj, _x[i], o );
        
        for (unsigned j = 0; j < o; j++) {
            gf256v_madd(y, trimat, _xixj[j], vec_len);
            trimat += vec_len;  
        }   
    }
 
    // P3
    for (unsigned i = 0; i < o; i++) {
        unsigned i_start = i - (i & 3);
        for (unsigned j = i; j < o; j++) {
            _xixj[j] = _x[v + j];
        }
         
        gfv_mul_scalar( _xixj + i_start, _x[v + i], o - i_start );
        
        for (unsigned j = i; j < o; j++) {
            gf256v_madd(y, trimat, _xixj[j], vec_len);
            trimat += vec_len;
        }  
    } 
} 