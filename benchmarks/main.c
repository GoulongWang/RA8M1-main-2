#ifndef UNIT_TEST

#ifdef RA8M1
#include "hal_data.h"
#elif defined(QEMU_M55)
#include "SSE300MPS3.h"
#endif

#include <inttypes.h>
#include "utils.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "randombytes.h"

#define TEST_RUN 100
#define REPEAT  1
#define gf16v_madd  _gf16v_madd_u32
#define gf256v_add  _gf256v_add_u32
#define gf256v_madd _gf256v_madd_u32

#define N 64
#define M 64
#define _MAX_N 256
#define TMPVEC_LEN 32
#define _PUB_N 64
#define _PUB_M 64
#define _V 32
#define _PUB_M_BYTE (_PUB_M / 2)
#define _GFSIZE 16


static volatile unsigned long long overflowcnt = 0;


void SysTick_Handler(void)
{
   ++overflowcnt;
}


uint64_t hal_get_time();

void PMU_Init();
void PMU_Finalize();
void PMU_Init_Status( pmu_stats *s );
void PMU_Finalize_Status( pmu_stats *s );
void PMU_Send_Status( char *s, pmu_stats const *stats );


#define BENCH_GF16MAT_PROD(fun, out, mat, vec, width) do {               \
    pmu_stats stats;                                                       \
    PMU_Init_Status(&stats);                                               \
    for (size_t cnt = 0; cnt < REPEAT; cnt++) {                          \
        fun(out, mat, vec, width); \
    }                                                                      \
    PMU_Finalize_Status(&stats);                                           \
    PMU_Send_Status(#fun, &stats);                                         \
    printf("stats.pmu_cycles: %" PRIu32 " cycles\n",                               \
                 stats.pmu_cycles);                     \
} while (0)

#define BENCH_GF16MAT_PROD_REF(fun, out, matA, n_A_vec_byte, n_A_width, b)  \
do {                                                                        \
    pmu_stats stats;                                                        \
    PMU_Init_Status(&stats);                                                \
    for (size_t cnt = 0; cnt < REPEAT; cnt++) {                           \
        fun(out, matA, n_A_vec_byte, n_A_width, b);                           \
    }                                                                       \
    PMU_Finalize_Status(&stats);                                            \
    PMU_Send_Status(#fun, &stats);                                          \
    printf(#fun ": %" PRIu32 " cycles (avg)\n",                                \
           stats.pmu_cycles);                               \
} while(0)

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
    } while (0)

void gf16mat_prod_2048_96(uint8_t *c, const uint8_t *matA, const uint8_t *b);
void gf16mat_prod_48_64(uint8_t *c, const uint8_t *matA, const uint8_t *b);
void gf16mat_prod_32_X(uint8_t *c, const uint8_t *matA, const uint8_t *b, size_t n_A_width);
void batch_2trimat_madd_gf16_mve( unsigned char *bC, const unsigned char *btriA,
    const unsigned char *B, unsigned Bheight, unsigned size_Bcolvec, unsigned Bwidth, unsigned size_batch );
void ov_publicmap_mve( unsigned char *y, const unsigned char *trimat, const unsigned char *x );

void gf16mat_prod_m4f_2048_96_normal_normal(uint32_t *c, uint32_t *a, uint8_t *b);
void gf16mat_prod_m4f_48_64_normal_normal(uint32_t *c, uint32_t *a, uint8_t *b);
void gf16mat_prod_m4f_32_X_normal_normal(uint8_t *c, const uint8_t *matA, const uint8_t *b, size_t n_A_width);
void gf16trimat_2trimat_madd_m4f_96_48_64_32(uint32_t *c, uint32_t *a, uint8_t *b);

void benchmark_gf16mat_prod_2048_96();
void benchmark_gf16mat_prod_48_64();
void benchmark_gf16mat_prod_32_X();
void benchmark_gf16mat_prod_32_X_MACRO();
void benchmark_gf16trimat_2trimat_madd_96_48_64_32();
void benchmark_ov_publicmap();




ITCM_FN int main(void) {
    Utils_Init();
    PMU_Init();
    benchmark_gf16mat_prod_32_X_MACRO();
    benchmark_gf16mat_prod_32_X();
    benchmark_ov_publicmap();
    benchmark_gf16mat_prod_2048_96();
    benchmark_gf16mat_prod_48_64();
    benchmark_gf16trimat_2trimat_madd_96_48_64_32();
    PMU_Finalize();
    return 0;
}

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

void batch_2trimat_madd_gf16( unsigned char *bC, const unsigned char *btriA,
    const unsigned char *B, unsigned Bheight, unsigned size_Bcolvec, unsigned Bwidth, unsigned size_batch ) {
#define MAX_O_BYTE (32)
#define MAX_V      (96)
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
            gf16mat_prod_ref( tmp_c, tmp_Arow, size_batch, Aheight, B + (j * size_Bcolvec) );
            gf256v_add( bC, tmp_c, size_batch);
            bC += size_batch;  
        }
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
    for(unsigned i=0;i<_num_byte;i++){
        a[i] = gf16_mul(a[i], gf16_b) | (gf16_mul(a[i]>>4, gf16_b) << 4);
    }
}

// different name of _gf16v_madd_u32, same function
void gf16v_madd_(uint8_t *a, const uint8_t *mat, uint8_t gf16_b, unsigned _num_byte) {
    for(unsigned i=0;i<_num_byte;i++){
        a[i] ^= gf16_mul(mat[i], gf16_b) | (gf16_mul(mat[i]>>4, gf16_b) << 4);
    }
}

void gf16v_add(uint8_t *a, const uint8_t *b, unsigned _num_byte) {
    for(unsigned i=0;i<_num_byte;i++){
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
            gf16v_madd_(y, trimat, _xixj[j], vec_len);
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
            gf16v_madd_(y, trimat, _xixj[j], vec_len);
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
            gf16v_madd_(y, trimat, _xixj[j], vec_len);
            trimat += vec_len;
        }
    }
}
// uov_publicmap() end ===================================================

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

void benchmark_gf16mat_prod_2048_96(){
    printf("=== UOV-Is: gf16mat_prod 2048_96 Unit Test ===\n");
    uint32_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint32_t cycles = 0;
    uint32_t N_A_VEC_BYTE = 2048, N_A_WIDTH = 96;

    uint8_t matA[ N_A_VEC_BYTE * N_A_WIDTH];
    uint8_t vec_b[N_A_VEC_BYTE ];
    uint8_t vec_c0[ N_A_VEC_BYTE ];
    uint8_t vec_c1[ N_A_VEC_BYTE ];
    uint8_t vec_c2[ N_A_VEC_BYTE ];
    
    int fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes(matA, sizeof matA);
        randombytes(vec_b, sizeof vec_b);
        memset(vec_c0, 0, sizeof(vec_c0));
        memset(vec_c1, 0, sizeof(vec_c1));
        memset(vec_c2, 0, sizeof(vec_c2));
        
        bench_cycles(gf16mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b ), cycles);
        sum_ref += cycles;
        bench_cycles(gf16mat_prod_2048_96( vec_c1, matA, vec_b ), cycles);    
        sum_mve += cycles;
        bench_cycles(gf16mat_prod_m4f_2048_96_normal_normal((uint32_t *)vec_c2, (uint32_t *)matA, vec_b), cycles);
        sum_m4 += cycles;
        
        if (memcmp(vec_c0, vec_c1, N_A_VEC_BYTE)) {
            fail = 1;
            break;
        }
    }

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
    printf("Average ref cycles = %lu\n", sum_ref / TEST_RUN);
    printf("Average MVE cycles = %lu\n", sum_mve / TEST_RUN);
    printf("Average M4  cycles = %lu\n", sum_m4 / TEST_RUN);
}

void benchmark_gf16mat_prod_48_64(){
    printf("\n\n=== UOV-Is: gf16mat_prod 48_64 Unit Test ===\n");
    uint32_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint32_t cycles = 0;
    uint8_t N_A_VEC_BYTE = 48, N_A_WIDTH = 64;
    uint8_t matA2[ N_A_VEC_BYTE * N_A_WIDTH];
    uint8_t vec_B[N_A_VEC_BYTE ];
    uint8_t vec_C0[ N_A_VEC_BYTE ];
    uint8_t vec_C1[ N_A_VEC_BYTE ];
    uint8_t vec_C2[ N_A_VEC_BYTE ];
    
    int fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes(matA2, sizeof matA2);
        randombytes(vec_B, sizeof vec_B);
        memset(vec_C0, 0, sizeof(vec_C0));
        memset(vec_C1, 0, sizeof(vec_C1));
        memset(vec_C2, 0, sizeof(vec_C2));
        
        bench_cycles(gf16mat_prod_ref( vec_C0, matA2, N_A_VEC_BYTE, N_A_WIDTH, vec_B ), cycles);
        sum_ref += cycles;
        bench_cycles(gf16mat_prod_48_64( vec_C1, matA2, vec_B ), cycles);    
        sum_mve += cycles;
        bench_cycles(gf16mat_prod_m4f_48_64_normal_normal((uint32_t*) vec_C2, (uint32_t*) matA2, vec_B ), cycles);    
        sum_m4 += cycles;
        
        if (memcmp(vec_C0, vec_C1, N_A_VEC_BYTE)) {
            fail = 1;
            break;
        }
    } 

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
    printf("Average ref cycles = %lu\n", sum_ref / TEST_RUN);
    printf("Average MVE cycles = %lu\n", sum_mve / TEST_RUN);
    printf("Average M4  cycles = %lu\n", sum_m4 / TEST_RUN);
}

void benchmark_gf16mat_prod_32_X(){
    printf("\n\n=== UOV-Is: gf16mat_prod 32_X Unit Test ===\n");
    uint32_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint32_t cycles = 0;
    uint8_t N_A_WIDTH_test, N_A_VEC_BYTE_test = 32;
    uint8_t out_ref[ N_A_VEC_BYTE_test ];
    uint8_t out_mve[ N_A_VEC_BYTE_test ];
    uint8_t out_m4[ N_A_VEC_BYTE_test ];

    int fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes((uint8_t*) &N_A_WIDTH_test, sizeof(uint8_t)); //
        N_A_WIDTH_test = N_A_WIDTH_test % 32 + 1; //
        uint8_t matA3[ N_A_VEC_BYTE_test * N_A_WIDTH_test ];
        uint8_t vec_B3[N_A_WIDTH_test / 2]; 
        randombytes(matA3, sizeof matA3);
        randombytes(vec_B3, sizeof vec_B3);
        memset(out_ref, 0, sizeof(out_ref));
        memset(out_mve, 0, sizeof(out_mve));
        memset(out_m4, 0, sizeof(out_m4));
        
        bench_cycles(gf16mat_prod_ref(out_ref, matA3, N_A_VEC_BYTE_test, N_A_WIDTH_test, vec_B3), cycles);
        sum_ref += cycles;
        bench_cycles(gf16mat_prod_32_X(out_mve, matA3, vec_B3, N_A_WIDTH_test), cycles);
        sum_mve += cycles;
        bench_cycles(gf16mat_prod_m4f_32_X_normal_normal(out_m4, matA3, vec_B3, N_A_WIDTH_test), cycles);
        sum_m4 += cycles;

        if (memcmp(out_ref, out_mve, N_A_VEC_BYTE_test)) {
            printf("out_ref = ");
            for (unsigned i = 0; i < sizeof(out_ref); i++) {
                printf("%02x ", out_ref[i]);
            }
            printf("\n");

            printf("out_mve = ");
            for (unsigned i = 0; i < sizeof(out_mve); i++) {
                printf("%02x ", out_mve[i]);
            }
            printf("\n");
            fail = 1;
            break;
        }
    }

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
    printf("Average ref cycles = %lu\n", sum_ref / TEST_RUN);
    printf("Average MVE cycles = %lu\n", sum_mve / TEST_RUN);
    printf("Average M4  cycles = %lu\n", sum_m4 / TEST_RUN);
}

void benchmark_gf16mat_prod_32_X_MACRO(){
    printf("\n\n=== UOV-Is: gf16mat_prod 32_X Unit Test, REPEAT: %d ===\n", REPEAT);
    uint8_t N_A_WIDTH_test = 32;
    uint8_t N_A_VEC_BYTE_test = 32;
    uint8_t out_ref[ N_A_VEC_BYTE_test ];
    uint8_t out_mve[ N_A_VEC_BYTE_test ];
    uint8_t out_m4[ N_A_VEC_BYTE_test ];
    

    uint8_t matA3[ N_A_VEC_BYTE_test * N_A_WIDTH_test ];
    uint8_t vec_B3[N_A_WIDTH_test / 2]; 
    randombytes(matA3, sizeof matA3);
    randombytes(vec_B3, sizeof vec_B3);
    memset(out_ref, 0, sizeof(out_ref));
    memset(out_mve, 0, sizeof(out_mve));
    memset(out_m4, 0, sizeof(out_m4));
    
    BENCH_GF16MAT_PROD_REF(gf16mat_prod_ref, out_ref, matA3, N_A_VEC_BYTE_test, N_A_WIDTH_test, vec_B3);
    BENCH_GF16MAT_PROD(gf16mat_prod_32_X, out_mve, matA3, vec_B3, 32);
    BENCH_GF16MAT_PROD(gf16mat_prod_m4f_32_X_normal_normal, out_m4, matA3, vec_B3, 32);
}

void benchmark_gf16trimat_2trimat_madd_96_48_64_32(){
    printf("\n\n=== UOV-Is: gf16trimat_2trimat_madd 96_48_64_32 Unit Test ===\n");
    uint64_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint32_t cycles = 0;
    uint8_t BHEIGHT = 96, SIZE_BCOLVEC = 48, BWIDTH = 64, SIZE_BATCH = 32;
    uint8_t btriA[SIZE_BATCH * BHEIGHT * (BHEIGHT + 1) / 2]; 
    uint8_t B[(BHEIGHT * BWIDTH) / 2];
    uint8_t bC[SIZE_BATCH * (BHEIGHT * BWIDTH)];
    uint8_t bC_mve[SIZE_BATCH * (BHEIGHT * BWIDTH)];
    

    int fail = 0;
    for (uint8_t l = 0; l < TEST_RUN; l++) {
        memset(bC, 0, sizeof(bC));
        memset(bC_mve, 0, sizeof(bC_mve));
        randombytes((uint8_t*) btriA, sizeof(btriA));
        randombytes((uint8_t*) B, sizeof(B));
        
        bench_cycles(gf16trimat_2trimat_madd_m4f_96_48_64_32((uint32_t*) bC, (uint32_t*)  btriA, (uint8_t*)B), cycles);
        sum_m4 += cycles;
    
        memset(bC, 0, sizeof(bC));
        bench_cycles(batch_2trimat_madd_gf16_mve(bC_mve, btriA, B, BHEIGHT, SIZE_BCOLVEC, BWIDTH, SIZE_BATCH), cycles);
        sum_mve += cycles;
        bench_cycles(batch_2trimat_madd_gf16(bC, btriA, B, BHEIGHT, SIZE_BCOLVEC, BWIDTH, SIZE_BATCH), cycles);
        sum_ref += cycles;
        printf("\n");
        
        if (memcmp(bC, bC_mve, sizeof(bC))) {
            printf("bc_ref = [");
            for (unsigned i = 0; i < sizeof(bC); i++) {
                printf("%02x ", bC[i]);
            }
            printf("]\n");
            printf("bc_mve = [");
            for (unsigned i = 0; i < sizeof(bC_mve); i++) {
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

void benchmark_ov_publicmap(){
    uint32_t sum_ref = 0, sum_mve = 0;
    uint32_t cycles;

    uint8_t P[M * N * (N + 1) / 4];
    uint8_t sig[N / 2];
    uint8_t acc[M / 2], acc_mve[M / 2];

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

uint64_t hal_get_time(){
  while (1) {
    unsigned long long before = overflowcnt;
    unsigned long long result = (before + 1) * 16777216llu - SysTick->VAL;
    if (overflowcnt == before) {
      return result;
    }
  }
}



#endif