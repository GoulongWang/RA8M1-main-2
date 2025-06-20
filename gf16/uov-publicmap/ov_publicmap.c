#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../randombytes.h"

#define TEST_RUN 100
#define N 64
#define M 64
#define _MAX_N 256
#define TMPVEC_LEN 32
#define _PUB_N 64
#define _PUB_M 64
#define _V 32
#define _PUB_M_BYTE (_PUB_M / 2)
#define _GFSIZE 16

void ov_publicmap( unsigned char *y, const unsigned char *trimat, const unsigned char *x );
void ov_publicmap_mve( unsigned char *y, const unsigned char *trimat, const unsigned char *x );

int main(){
    printf("=== GF16 ov_publicmap Unit Test ===\n");
    uint8_t P[M * N * (N + 1) / 4];
    uint8_t sig[N / 2];
    uint8_t acc[M / 2], acc_mve[M / 2];

    int fail = 0;
    for (int i = 0; i < TEST_RUN; i++) {
        memset(acc, 0, sizeof(acc));
        memset(acc_mve, 0, sizeof(acc_mve));
        randombytes(P, sizeof(P));
        randombytes(sig, sizeof sig);

        ov_publicmap(acc, P, sig);
        ov_publicmap_mve(acc_mve, P, sig);

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
}