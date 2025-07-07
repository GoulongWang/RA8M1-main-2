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
#define BHEIGHT      68
#define SIZE_BCOLVEC 68
#define BWIDTH       44
#define SIZE_BATCH   44

void gf256mat_prod_1936_68(uint8_t *c, const uint8_t *matA, const uint8_t *b);
void gf256mat_prod_68_44(uint8_t *c, const uint8_t *matA, const uint8_t *b);
void gf256mat_prod_44_X(uint8_t *c, const uint8_t *matA, const uint8_t *b, size_t n_A_width);
void batch_2trimat_madd_gf256_mve( unsigned char *bC, const unsigned char *btriA,
    const unsigned char *B, unsigned Bheight, unsigned size_Bcolvec, unsigned Bwidth, unsigned size_batch );

void gf256mat_prod_m4f_1936_68_normal_normal(uint32_t *c, uint32_t *a, uint8_t *b);
void gf256mat_prod_m4f_68_44_normal_normal(uint32_t *c, uint32_t *a, uint8_t *b);
void gf256mat_prod_m4f_44_X_normal_normal(uint32_t *c, uint32_t *a, uint8_t *b, size_t n_A_width);
void gf256trimat_2trimat_madd_m4f_68_68_44_44(uint32_t *c, uint32_t *a, uint8_t *b);

void benchmark_gf256mat_prod_1936_68();
void benchmark_gf256mat_prod_68_44(); 
void benchmark_gf256mat_prod_44_X();
void benchmark_gf256trimat_2trimat_madd_68_68_44_44();

ITCM_FN int main (void)
{
    Utils_Init();
    PMU_Init();
    benchmark_gf256mat_prod_1936_68();
    benchmark_gf256mat_prod_68_44(); 
    benchmark_gf256mat_prod_44_X(); 
    benchmark_gf256trimat_2trimat_madd_68_68_44_44();
    return( 0 );
}

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

void benchmark_gf256mat_prod_1936_68(){
    printf("====== UOV-Ip: gf256mat_prod 1936_68 unit test ======\n");
    uint32_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint32_t cycles;

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

        bench_cycles3(gf256mat_prod_m4f_1936_68_normal_normal((uint32_t *)vec_c0, (uint32_t *) matA, vec_b), cycles);
        sum_m4 += cycles;
        memset(vec_c0, 0, sizeof(vec_c0));
        bench_cycles3(gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b), cycles);
        sum_ref += cycles;
        bench_cycles3(gf256mat_prod_1936_68(vec_c1, matA, vec_b), cycles);
        sum_mve += cycles;

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

    bench_cycles(gf256mat_prod_m4f_1936_68_normal_normal((uint32_t *)vec_c0, (uint32_t *) matA, vec_b), cycles);
    bench_cycles(gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b), cycles);
    bench_cycles(gf256mat_prod_1936_68(vec_c1, matA, vec_b), cycles);
    
    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
    printf("Average ref cycles = %lu\n", sum_ref / TEST_RUN);
    printf("Average MVE cycles = %lu\n", sum_mve / TEST_RUN);
    printf("Average M4 cycles = %lu\n", sum_m4 / TEST_RUN);
}

void benchmark_gf256mat_prod_68_44(){
    printf("\n\n====== UOV-Ip: gf256mat_prod 68_44 unit test ======\n");
    uint32_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint32_t cycles;

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

        bench_cycles3(gf256mat_prod_m4f_68_44_normal_normal((uint32_t *) vec_c0, (uint32_t *) matA, vec_b), cycles);
        sum_m4 += cycles;

        memset(vec_c0, 0, sizeof(vec_c0));
        bench_cycles3(gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b), cycles);
        sum_ref += cycles;
        bench_cycles3(gf256mat_prod_68_44(vec_c1, matA, vec_b), cycles);
        sum_mve += cycles;

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

    bench_cycles(gf256mat_prod_m4f_68_44_normal_normal((uint32_t *) vec_c0, (uint32_t *) matA, vec_b), cycles);
    bench_cycles(gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b), cycles);
    bench_cycles(gf256mat_prod_68_44(vec_c1, matA, vec_b), cycles);
    
    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
    printf("Average ref cycles = %lu\n", sum_ref / TEST_RUN);
    printf("Average MVE cycles = %lu\n", sum_mve / TEST_RUN);
    printf("Average M4 cycles = %lu\n", sum_m4 / TEST_RUN);
} 

void benchmark_gf256mat_prod_44_X(){
    printf("\n\n====== UOV-Ip: gf256mat_prod 44_X unit test ======\n");
    uint32_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint32_t cycles;

    uint8_t N_A_VEC_BYTE = 44, N_A_WIDTH = 44;
    uint8_t vec_b[ N_A_VEC_BYTE ];
    uint8_t vec_c0[ N_A_VEC_BYTE ];
    uint8_t vec_c1[ N_A_VEC_BYTE ];
    uint8_t vec_c2[ N_A_VEC_BYTE ];
    uint8_t matA[ N_A_WIDTH * N_A_VEC_BYTE];

    int fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes(vec_b, sizeof vec_b);
        //randombytes(&N_A_WIDTH, sizeof(N_A_WIDTH));
        //N_A_WIDTH = N_A_WIDTH % 44 + 1;    
        randombytes(matA, sizeof matA);

        memset(vec_c0, 0, sizeof(vec_c0));
        memset(vec_c1, 0, sizeof(vec_c1));
        memset(vec_c2, 0, sizeof(vec_c2));

        bench_cycles3(gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b), cycles);
        sum_ref += cycles;
        bench_cycles3(gf256mat_prod_44_X(vec_c1, matA, vec_b, N_A_WIDTH), cycles);
        sum_mve += cycles;
        bench_cycles3(gf256mat_prod_m4f_44_X_normal_normal((uint32_t *)vec_c2, (uint32_t *)matA, vec_b, N_A_WIDTH), cycles);
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

    bench_cycles(gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b), cycles);
    bench_cycles(gf256mat_prod_44_X(vec_c1, matA, vec_b, N_A_WIDTH), cycles);
    bench_cycles(gf256mat_prod_m4f_44_X_normal_normal((uint32_t *)vec_c2, (uint32_t *)matA, vec_b, N_A_WIDTH), cycles);
    
    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
    printf("Average ref cycles = %lu\n", sum_ref / TEST_RUN);
    printf("Average MVE cycles = %lu\n", sum_mve / TEST_RUN);
    printf("Average M4 cycles = %lu\n", sum_m4 / TEST_RUN);
}

void benchmark_gf256trimat_2trimat_madd_68_68_44_44(){
    printf("\n\n=== UOV-Ip: gf256trimat_2trimat_madd 68_68_44_44 Unit Test ===\n");
    uint64_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint64_t cycles;

    uint8_t btriA[SIZE_BATCH * BHEIGHT * (BHEIGHT + 1) / 2]; 
    uint8_t B[BHEIGHT * BWIDTH];
    uint8_t bC[SIZE_BATCH * (BHEIGHT * BWIDTH)];
    uint8_t bC_mve[SIZE_BATCH * (BHEIGHT * BWIDTH)];

    int fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        memset(bC, 0, sizeof(bC));
        memset(bC_mve, 0, sizeof(bC_mve));
        randombytes((uint8_t*) btriA, sizeof(btriA));
        randombytes((uint8_t*) B, sizeof(B));
        
        bench_cycles3(gf256trimat_2trimat_madd_m4f_68_68_44_44((uint32_t *)bC, (uint32_t *) btriA, B), cycles);
        sum_m4 += cycles;
        // batch_2trimat_madd( S, P1, sk_O, _V, _V_BYTE, _O, _O_BYTE )
        memset(bC, 0, sizeof(bC));
        bench_cycles3(batch_2trimat_madd_gf256(bC, btriA, B, BHEIGHT, SIZE_BCOLVEC, BWIDTH, SIZE_BATCH), cycles);
        sum_ref += cycles;
        bench_cycles3(batch_2trimat_madd_gf256_mve(bC_mve, btriA, B, BHEIGHT, SIZE_BCOLVEC, BWIDTH, SIZE_BATCH), cycles);
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

    bench_cycles(gf256trimat_2trimat_madd_m4f_68_68_44_44((uint32_t *)bC, (uint32_t *) btriA, B), cycles);
    bench_cycles(batch_2trimat_madd_gf256(bC, btriA, B, BHEIGHT, SIZE_BCOLVEC, BWIDTH, SIZE_BATCH), cycles);
    bench_cycles(batch_2trimat_madd_gf256_mve(bC_mve, btriA, B, BHEIGHT, SIZE_BCOLVEC, BWIDTH, SIZE_BATCH), cycles);
    
    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n"); 
    printf("Average ref cycles = ");
    print_u64(sum_ref / TEST_RUN);
    printf("Average MVE cycles = ");
    print_u64(sum_mve / TEST_RUN);
    printf("Average M4  cycles = ");
    print_u64(sum_m4 / TEST_RUN);
}
#endif