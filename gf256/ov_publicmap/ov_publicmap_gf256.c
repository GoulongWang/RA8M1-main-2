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
#define gf256v_add     _gf256v_add_u32
#define gf256v_madd    _gf256v_madd_u32
void ov_publicmap( unsigned char *y, const unsigned char *trimat, const unsigned char *x );
//void ov_publicmap_mve( unsigned char *y, const unsigned char *trimat, const unsigned char *x );

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
        //ov_publicmap_mve(acc_mve, P, sig);
        
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

static
void accu_eval_quad_gf256( unsigned char *accu_low, unsigned char *accu_high, const unsigned char *trimat, const unsigned char *x_in_byte) {
    unsigned vec_len = _PUB_M_BYTE;
    const unsigned char *_x = x_in_byte;
    unsigned char _xixj[_MAX_N];
    unsigned v = _V;
    unsigned o = _PUB_N - _V;
    unsigned n = _PUB_N;

    unsigned tmpvec_len = ((vec_len + 3) >> 2) << 2;
    
    // P1
    for (unsigned i = 0; i < v; i++) {
        unsigned i_start = i - (i & 3);
        for (unsigned j = i; j < v; j++) {
            _xixj[j] = _x[j];
        }
        gfv_mul_scalar( _xixj + i_start, _x[i], v - i_start );
        unsigned j = i;
        for (; j < v; j++) {
            unsigned idx = _xixj[j];
            gf256v_add( accu_low  + TMPVEC_LEN * (idx & 0xf), trimat, tmpvec_len );
            gf256v_add( accu_high  + TMPVEC_LEN * (idx >> 4), trimat, tmpvec_len );
            trimat += vec_len;
        }
    }
    // P2
    for (unsigned i = 0; i < v; i++) {
        for (unsigned j = 0; j < o; j++) {
            _xixj[j] = _x[v + j];
        }
        gfv_mul_scalar( _xixj, _x[i], o );
        unsigned j = 0;
        for (; j < o; j++) {
            unsigned idx = _xixj[j];
            gf256v_add( accu_low  + TMPVEC_LEN * (idx & 0xf), trimat, tmpvec_len );
            gf256v_add( accu_high  + TMPVEC_LEN * (idx >> 4), trimat, tmpvec_len );
            trimat += vec_len;
        }
    }
    // P3
    for (unsigned i = v; i < n - 1; i++) {
        unsigned i_start = i - (i & 3);
        for (unsigned j = i; j < n; j++) {
            _xixj[j] = _x[j];
        }
        gfv_mul_scalar( _xixj + i_start, _x[i], n - i_start );
        unsigned j = i;
        for (; j < n; j++) {
            unsigned idx = _xixj[j];
            gf256v_add( accu_low  + TMPVEC_LEN * (idx & 0xf), trimat, tmpvec_len );
            gf256v_add( accu_high  + TMPVEC_LEN * (idx >> 4), trimat, tmpvec_len );
            trimat += vec_len;
        }
    }
    for (unsigned i = n - 1; i < n; i++) {
        unsigned i_start = i - (i & 3);
        for (unsigned j = i; j < n; j++) {
            _xixj[j] = _x[j];
        }
        
        gfv_mul_scalar( _xixj + i_start, _x[i], n - i_start );
        for (unsigned j = i; j < n; j++) {
            unsigned idx = _xixj[j];
            gf256v_add( accu_low  + TMPVEC_LEN * (idx & 0xf), trimat, vec_len );
            gf256v_add( accu_high + TMPVEC_LEN * (idx >> 4), trimat, vec_len );
            trimat += vec_len;
        }
    }

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

static
void madd_reduce_gf256( unsigned char *y, unsigned char *tmp_low, unsigned char *tmp_high, unsigned vec_len ) {
    unsigned tmpvec_len = ((vec_len + 3) >> 2) << 2;
    unsigned char tmp_y[TMPVEC_LEN * 4];

    for (int i = 15; i > 8; i--) {
        gf256v_add( tmp_low + TMPVEC_LEN * 8, tmp_low + TMPVEC_LEN * i, tmpvec_len );
        gf256v_add( tmp_low + TMPVEC_LEN * (i - 8), tmp_low + TMPVEC_LEN * i, tmpvec_len );
    }
    for (int i = 7; i > 4; i--) {
        gf256v_add( tmp_low + TMPVEC_LEN * 4, tmp_low + TMPVEC_LEN * i, tmpvec_len );
        gf256v_add( tmp_low + TMPVEC_LEN * (i - 4), tmp_low + TMPVEC_LEN * i, tmpvec_len );
    }
    for (int i = 3; i > 2; i--) {
        gf256v_add( tmp_low + TMPVEC_LEN * 2, tmp_low + TMPVEC_LEN * i, tmpvec_len );
        gf256v_add( tmp_low + TMPVEC_LEN * (i - 2), tmp_low + TMPVEC_LEN * i, tmpvec_len );
    }
    gf256v_set_zero( tmp_y, tmpvec_len );
    gf256v_add ( tmp_y, tmp_low + TMPVEC_LEN, tmpvec_len );  // x1
    gf256v_madd( tmp_y, tmp_low + TMPVEC_LEN * 2, 2, tmpvec_len ); // x2
    gf256v_madd( tmp_y, tmp_low + TMPVEC_LEN * 4, 4, tmpvec_len ); // x4
    gf256v_madd( tmp_y, tmp_low + TMPVEC_LEN * 8, 8, tmpvec_len ); // x8

    for (int i = 15; i > 8; i--) {
        gf256v_add( tmp_high + TMPVEC_LEN * 8, tmp_high + TMPVEC_LEN * i, tmpvec_len );
        gf256v_add( tmp_high + TMPVEC_LEN * (i - 8), tmp_high + TMPVEC_LEN * i, tmpvec_len );
    }
    for (int i = 7; i > 4; i--) {
        gf256v_add( tmp_high + TMPVEC_LEN * 4, tmp_high + TMPVEC_LEN * i, tmpvec_len );
        gf256v_add( tmp_high + TMPVEC_LEN * (i - 4), tmp_high + TMPVEC_LEN * i, tmpvec_len );
    }
    for (int i = 3; i > 2; i--) {
        gf256v_add( tmp_high + TMPVEC_LEN * 2, tmp_high + TMPVEC_LEN * i, tmpvec_len );
        gf256v_add( tmp_high + TMPVEC_LEN * (i - 2), tmp_high + TMPVEC_LEN * i, tmpvec_len );
    }
    gf256v_madd( tmp_y, tmp_high + TMPVEC_LEN * 1, 16, tmpvec_len ); // x16
    gf256v_madd( tmp_y, tmp_high + TMPVEC_LEN * 2, 32, tmpvec_len ); // x32
    gf256v_madd( tmp_y, tmp_high + TMPVEC_LEN * 4, 64, tmpvec_len ); // x64
    gf256v_madd( tmp_y, tmp_high + TMPVEC_LEN * 8, 128, tmpvec_len ); // x128

    memcpy( y, tmp_y, vec_len );
}

void ov_publicmap( unsigned char *y, const unsigned char *trimat, const unsigned char *x ) {
    unsigned char _x[_MAX_N];
    for (unsigned i = 0; i < _PUB_N; i++) {
        _x[i] = gf256v_get_ele( x, i );
    }

    unsigned char tmp_l[TMPVEC_LEN * 16] = {0};
    unsigned char tmp_h[TMPVEC_LEN * 16] = {0};
    accu_eval_quad_gf256( tmp_l, tmp_h, trimat, _x );
    madd_reduce_gf256( y, tmp_l, tmp_h, _PUB_M_BYTE );
}

/* 
static inline uint8_t gfv_get_ele(const uint8_t *a, unsigned i) {
    uint8_t r = a[i >> 1];
    return (i & 1) ? (r >> 4) : (r & 0xf);
}

// gf16 := gf2[x]/(x^4+x+1)
static inline uint8_t gf16_mul(uint8_t a, uint8_t b) {
    uint8_t r8 = (a & 1) * b;
    r8 ^= (a & 2) * b;
    r8 ^= (a & 4) * b;
    r8 ^= (a & 8) * b;

    // reduction
    uint8_t r4 = r8 ^ (((r8 >> 4) & 5) * 3); // x^4 = x+1  , x^6 = x^3 + x^2
    r4 ^= (((r8 >> 5) & 1) * 6);       // x^5 = x^2 + x
    return (r4 & 0xf);
}

void gf16v_mul(uint8_t *a, uint8_t gf16_b, unsigned _num_byte) {
    for(int i=0;i<_num_byte;i++){
        a[i] = gf16_mul(a[i], gf16_b) | (gf16_mul(a[i]>>4, gf16_b) << 4);
    }
}

void gf16v_madd(uint8_t *a, const uint8_t *mat, uint8_t gf16_b, unsigned _num_byte) {
    for(int i=0;i<_num_byte;i++){
        a[i] ^= gf16_mul(mat[i], gf16_b) | (gf16_mul(mat[i]>>4, gf16_b) << 4);
    }
}

void gf16v_add(uint8_t *a, const uint8_t *b, unsigned _num_byte) {
    for(int i=0;i<_num_byte;i++){
        a[i] ^= b[i];
    }
}

void ov_publicmap( unsigned char *y, const unsigned char *trimat, const unsigned char *x ) {
    unsigned char _xixj[_MAX_N] = {0};
    unsigned v = _V;
    unsigned o = _PUB_N - _V;
    unsigned char _x[_MAX_N];
    
    for (unsigned i = 0; i < _PUB_N; i++) {
        _x[i] = gfv_get_ele( x, i );
    }
    
    unsigned int vec_len = _PUB_M_BYTE;

    // P1
    for (unsigned i = 0; i < _V; i++) {
        unsigned i_start = i - (i & 3);
        for (unsigned j = i; j < _V; j++) {
            _xixj[j] = _x[j];
        }

        gf16v_mul( _xixj + i_start, _x[i], v - i_start );
        
        for (unsigned j = i; j < v; j++) {
            gf16v_madd(y, trimat, _xixj[j], vec_len);
            trimat += vec_len;
        }
    }   
        
    // P2
    for (unsigned i = 0; i < v; i++) {
        for (unsigned j = 0; j < o; j++) {
            _xixj[j] = _x[v + j];
        }   
        
        gf16v_mul( _xixj, _x[i], o );

        for (unsigned j = 0; j < o; j++) {
            gf16v_madd(y, trimat, _xixj[j], vec_len);
            trimat += vec_len;  
        }  
    }
 
    // P3
    for (unsigned i = 0; i < o; i++) {
        unsigned i_start = i - (i & 3);
        for (unsigned j = i; j < o; j++) {
            _xixj[j] = _x[v + j];
        }
         
        gf16v_mul( _xixj + i_start, _x[v + i], o - i_start );
        
        for (unsigned j = i; j < o; j++) {
            gf16v_madd(y, trimat, _xixj[j], vec_len);
            trimat += vec_len;
        } 
    } 
} */