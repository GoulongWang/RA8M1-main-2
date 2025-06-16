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
#include <stdlib.h>
#include <string.h>
#include "randombytes.h"
#include "utils.h"

#define gf256v_add _gf256v_add_u32
#define gf256v_madd _gf256v_madd_u32

#define TEST_RUN 100
#define N_A_VEC_BYTE 1936 //68//1936
#define N_A_WIDTH    68   //44//68

#define BHEIGHT      68
#define SIZE_BCOLVEC 68
#define BWIDTH       44
#define SIZE_BATCH   44

// uov_publicmap settings
#define N 64
#define M 64
#define _MAX_N 256
#define TMPVEC_LEN 32
#define _PUB_N 64
#define _PUB_M 64
#define _V 32
#define _PUB_M_BYTE (_PUB_M / 2)
#define _GFSIZE 16

#define bench_cycles(CALL, OUT_VAR)                                    \
    do {                                                               \
        __disable_irq();                                               \
        ARM_PMU_CYCCNT_Reset();                                        \
        ARM_PMU_CNTR_Enable(PMU_CNTENSET_CCNTR_ENABLE_Msk);            \
        CALL;                                                          \
        ARM_PMU_CNTR_Disable(PMU_CNTENCLR_CCNTR_ENABLE_Msk);           \
        printf(#CALL ": cycles = %" PRIu32 "\n", ARM_PMU_Get_CCNTR()); \
        OUT_VAR = ARM_PMU_Get_CCNTR();                                 \
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

// uov_publicmap() start ===================================================
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
\
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
// uov_publicmap() end ===================================================

void gf256mat_prod_1936_68(uint8_t *c, const uint8_t *matA, const uint8_t *b);
void gf256mat_prod_68_44(uint8_t *c, const uint8_t *matA, const uint8_t *b);
void gf256mat_prod_44_X(uint8_t *c, const uint8_t *matA, const uint8_t *b, size_t n_A_width);

void gf256mat_prod_m4f_1936_68_normal_normal(uint32_t *c, uint32_t *a, uint8_t *b);
void gf256mat_prod_m4f_68_44_normal_normal(uint32_t *c, uint32_t *a, uint8_t *b);
void gf256mat_prod_m4f_44_X_normal_normal(uint32_t *c, uint32_t *a, uint8_t *b, size_t n_A_width);
void gf256trimat_2trimat_madd_m4f_68_68_44_44(uint32_t *c, uint32_t *a, uint8_t *b);

void batch_2trimat_madd_gf256_mve( unsigned char *bC, const unsigned char *btriA,
    const unsigned char *B, unsigned Bheight, unsigned size_Bcolvec, unsigned Bwidth, unsigned size_batch );

void batch_2trimat_madd_gf256( unsigned char *bC, const unsigned char *btriA,
    const unsigned char *B, unsigned Bheight, unsigned size_Bcolvec, unsigned Bwidth, unsigned size_batch ) {
#define MAX_O_BYTE  (96)
#define MAX_V      (148)
    uint8_t tmp_c[MAX_O_BYTE];
    uint8_t tmp_Arow[MAX_V * MAX_O_BYTE];
#undef MAX_O_BYTE
#undef MAX_V

    // access fixed positions of destination matrix C
    unsigned Aheight = Bheight;
    for (unsigned i = 0; i < Aheight; i++) {
        const uint8_t *ptr = btriA + i * size_batch;
        for (unsigned j = 0; j < i; j++) {
            memcpy( tmp_Arow + j * size_batch, ptr, size_batch );  
            ptr += (Aheight - j - 1) * size_batch;
        }
        memset( tmp_Arow + i * size_batch, 0, size_batch );
        ptr += size_batch;
        memcpy( tmp_Arow + (i + 1)*size_batch, ptr, size_batch * (Aheight - i - 1) );
        
        for (unsigned j = 0; j < Bwidth; j++) {
            gf256mat_prod_ref( tmp_c, tmp_Arow, size_batch, Aheight, B + j * size_Bcolvec );   
            gf256v_add( bC, tmp_c, size_batch);
            bC += size_batch; 
        }
    }
}

static void print_u64(uint64_t v){
    /* 1 000 000 000 < 2^32，可安全存進 uint32_t */
    const uint32_t base = 1000000000U;

    if (v >= base) {
        uint32_t hi = (uint32_t)(v / base);
        uint32_t lo = (uint32_t)(v % base);
        /* lo 需補零到 9 位，才能跟 hi 無縫接起來 */
        printf("%lu%09lu\n", hi, lo);
    } else {
        printf("%lu\n", (uint32_t)v);
    }
}

void benchmark_gf256trimat_2trimat_madd_68_68_44_44(){
    printf("=== UOV-Ip: gf256trimat_2trimat_madd 68_68_44_44 Unit Test ===\n");
    uint64_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint64_t cycles;

    unsigned char btriA[SIZE_BATCH * BHEIGHT * (BHEIGHT + 1) / 2]; 
    unsigned char B[BHEIGHT * BWIDTH];
    unsigned char bC[SIZE_BATCH * (BHEIGHT * BWIDTH)];
    unsigned char bC_mve[SIZE_BATCH * (BHEIGHT * BWIDTH)];

    int fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        memset(bC, 0, sizeof(bC));
        memset(bC_mve, 0, sizeof(bC_mve));
        randombytes((uint8_t*) btriA, sizeof(btriA));
        randombytes((uint8_t*) B, sizeof(B));
        
        bench_cycles(gf256trimat_2trimat_madd_m4f_68_68_44_44((uint32_t *)bC, (uint32_t *) btriA, B), cycles);
        sum_m4 += cycles;
        // batch_2trimat_madd( S, P1, sk_O, _V, _V_BYTE, _O, _O_BYTE )
        memset(bC, 0, sizeof(bC));
        bench_cycles(batch_2trimat_madd_gf256(bC, btriA, B, BHEIGHT, SIZE_BCOLVEC, BWIDTH, SIZE_BATCH), cycles);
        sum_ref += cycles;
        bench_cycles(batch_2trimat_madd_gf256_mve(bC_mve, btriA, B, BHEIGHT, SIZE_BCOLVEC, BWIDTH, SIZE_BATCH), cycles);
        sum_mve += cycles;

        if (memcmp(bC, bC_mve, sizeof(bC))) {
            printf("bc_ref = [");
            int size = sizeof(bC);
            for (int i = 0; i < size; i++) {
                printf("%02x ", bC[i]);
            }
            printf("]\n");
            printf("bc_mve = [");
            for (int i = 0; i < size; i++) {
                printf("%02x ", bC_mve[i]);
            }
            printf("]\n");
            fail = 1;
            break;
        }
    }

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n"); 
    printf("Average ref cycles = ");
    print_u64(sum_ref / TEST_RUN);
    printf("Average MVE cycles = ");
    print_u64(sum_mve / TEST_RUN);
    printf("Average M4  cycles = ");
    print_u64(sum_m4 / TEST_RUN);
}

void ov_publicmap_mve( unsigned char *y, const unsigned char *trimat, const unsigned char *x );

void benchmark_ov_publicmap(){
    uint32_t sum_ref = 0, sum_mve = 0;
    uint32_t cycles;

    unsigned char P[M * N * (N + 1) / 4];
    unsigned char sig[N / 2];
    unsigned char acc[M / 2], acc_mve[M / 2];

    int fail = 0;
    for (int i = 0; i < TEST_RUN; i++) {
        memset(acc, 0, sizeof(acc));
        memset(acc_mve, 0, sizeof(acc_mve));
        randombytes(P, sizeof(P));
        randombytes(sig, sizeof sig);

        bench_cycles(ov_publicmap(acc, P, sig), cycles);
        sum_ref += cycles;
        bench_cycles(ov_publicmap_mve(acc_mve, P, sig), cycles);
        sum_mve += cycles;

        if(memcmp(acc_mve, acc, sizeof(acc))){
            printf("acc_ref = [");
            for (unsigned k = 0; k < sizeof(acc); k++) {
                printf("%02x ", acc[k]);
            }
            printf("]\n");

            printf("acc_mve = [");
            for (unsigned k = 0; k < sizeof(acc_mve); k++) {
                printf("%02x ", acc_mve[k]);
            }
            printf("]\n"); 
            fail = 1;
            break;
        }   
    } 

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
    printf("Average ref cycles = %lu\n", sum_ref / TEST_RUN);
    printf("Average MVE cycles = %lu\n", sum_mve / TEST_RUN);
}

void benchmark_gf256mat_prod_44_X(){
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
        //bench_cycles(gf256mat_prod_m4f_44_X_normal_normal((uint32_t *)vec_c2, (uint32_t *)matA, vec_b, N_A_WIDTH_test), cycles);
        // the code size of gf256mat_prod_m4f_44_X_normal_normal is 20000
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
}

ITCM_FN int main (void)
{
    Utils_Init();
    PMU_Init();
    //benchmark_gf256mat_prod_1936_68();// the code size of gf256mat_prod_m4f_1936_68_normal_normal is about 20000
    //benchmark_gf256mat_prod_68_44();  // the code size of gf256mat_prod_m4f_68_44_normal_normal is about 20000 
    benchmark_gf256mat_prod_44_X();   // the code size of gf256mat_prod_m4f_44_X_normal_normal is about 20000
    benchmark_gf256trimat_2trimat_madd_68_68_44_44();
    benchmark_ov_publicmap();
    return( 0 );
}
#endif