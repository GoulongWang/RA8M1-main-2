#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../randombytes.h"

#define TEST_RUN        100
#define M_VEC_LIMBS_MAX 5
#define M_VEC_LIMBS     5
#define MAT_ROWS        10
#define MAT_COLS        78
#define BS_MAT_COLS     8

static void pad_matrix(uint64_t *out, uint64_t *in,  const int mat_cols, const int bs_mat_cols);
static int mem_compare(uint64_t *acc_ref, uint64_t *acc);
void mul_add_mat_x_m_mat_mve(const int m_vec_limbs, const unsigned char *mat, const uint64_t *bs_mat, uint64_t *acc, 
                            const int mat_rows, const int mat_cols, const int bs_mat_cols);
static inline void mul_add_mat_x_m_mat_ref(const int m_vec_limbs, const unsigned char *mat, const uint64_t *bs_mat, uint64_t *acc,
                            const int mat_rows, const int mat_cols, const int bs_mat_cols);

int main (void)
{
    printf("=== GF16 MAYO Matrix Multiplication ===\n");
    unsigned char mat[MAT_ROWS * MAT_COLS];
    uint8_t bs_mat_bytes[MAT_COLS * BS_MAT_COLS * (M_VEC_LIMBS * sizeof(uint64_t))];
    uint8_t bs_mat_bytes_padded[MAT_COLS * BS_MAT_COLS * ((M_VEC_LIMBS + 1) * sizeof(uint64_t))]; // (M_VEC_LIMBS + 1) 多加一個 limb 來處理超過的部分

    uint64_t acc_ref[MAT_ROWS * BS_MAT_COLS * M_VEC_LIMBS];
    uint64_t acc[MAT_ROWS * BS_MAT_COLS * (M_VEC_LIMBS + 1)] __attribute__((aligned(8))); // 輸出的部分也需要調整成多一個 limb，但這個 limb 其實沒有用途

    int fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        random_4bits_in_a_byte(mat, sizeof mat);                    // (0000 xxxx) in a byte
        random_4bits_in_a_byte(bs_mat_bytes, sizeof(bs_mat_bytes)); // 1 bytes for 1 gf16 element

        memset(bs_mat_bytes_padded, 0, sizeof bs_mat_bytes_padded);
        // 把 bs_mat_bytes 矩陣擴展成符合 6 個 limbs 格式的矩陣
        pad_matrix((uint64_t *)bs_mat_bytes_padded, (uint64_t *)bs_mat_bytes, MAT_COLS, BS_MAT_COLS);

        uint64_t* bs_mat = (uint64_t*) bs_mat_bytes;

        memset(acc_ref, 0, sizeof acc_ref);
        memset(acc, 0, sizeof acc);

        mul_add_mat_x_m_mat_ref(M_VEC_LIMBS, mat, bs_mat, acc_ref, MAT_ROWS, MAT_COLS, BS_MAT_COLS);
        // 輸入的部分就要改成擴充後的 bs_mat_bytes_padded
        mul_add_mat_x_m_mat_mve(M_VEC_LIMBS + 1, mat, (uint64_t*) bs_mat_bytes_padded, acc, MAT_ROWS, MAT_COLS, BS_MAT_COLS);

        if (mem_compare(acc_ref, acc)) {
            break;
        }
    }

    printf((fail) ? "TEST FAIL!\n" : "TEST PASS.\n");

    return( 0 );
}

// This implements arithmetic for nibble-packed vectors of m field elements in Z_2[x]/(x^4+x+1)
// gf16 := gf2[x]/(x^4+x+1)
static inline uint32_t mul_table(uint8_t b){
    uint32_t x = ((uint32_t) b) * 0x08040201;
    uint32_t high_nibble_mask = 0xf0f0f0f0;
    uint32_t high_half = x & high_nibble_mask;
    return (x ^ (high_half >> 4) ^ (high_half >> 3)); // reduction
}

static inline void m_vec_mul_add (int m_vec_limbs, const uint64_t *in, unsigned char a, uint64_t *acc) {
    (void) m_vec_limbs;
    uint32_t tab = mul_table(a);
    uint64_t lsb_ask = 0x1111111111111111ULL;

    for(int i=0; i < M_VEC_LIMBS_MAX ;i++){
        acc[i] ^= ( in[i]       & lsb_ask) * (tab & 0xf)    
                ^ ((in[i] >> 1) & lsb_ask) * ((tab >> 8)  & 0xf)
                ^ ((in[i] >> 2) & lsb_ask) * ((tab >> 16) & 0xf)
                ^ ((in[i] >> 3) & lsb_ask) * ((tab >> 24) & 0xf);
    }
}

// multiplies a single matrix with m matrices and adds result to acc
static inline void mul_add_mat_x_m_mat_ref(const int m_vec_limbs, const unsigned char *mat, const uint64_t *bs_mat, uint64_t *acc,
                         const int mat_rows, const int mat_cols, const int bs_mat_cols) {
    for (int r = 0; r < mat_rows; r++) {
        for (int c = 0; c < mat_cols; c++) {
            for (int k = 0; k < bs_mat_cols; k += 1) {
                m_vec_mul_add(m_vec_limbs, bs_mat + m_vec_limbs * (c * bs_mat_cols + k), mat[r * mat_cols + c], acc + m_vec_limbs * (r * bs_mat_cols + k));
            }
        }
    }
}

// 把 matrix 多增加 padding，因為使用 128 bit vector 會多一個 limb
// uint8_t bs_mat_bytes_padded[MAT_COLS * BS_MAT_COLS * ((M_VEC_LIMBS + 1) * sizeof(uint64_t))];
static void pad_matrix(uint64_t *out, uint64_t *in,  const int mat_cols, const int bs_mat_cols){
    for (int c = 0; c < mat_cols; c++) {
        for (int k = 0; k < bs_mat_cols; k += 1) {
            for(int i = 0; i < 5; i++){
                out[6*(c * bs_mat_cols + k) + i] = in[5*(c * bs_mat_cols + k) + i];
                /*
                這樣看 out[（新起始位置) + i] = in[(起始位置) + i];

                bs_mat + m_vec_limbs * (c * bs_mat_cols + k) = bs_mat[m_vec_limbs * (c * bs_mat_cols + k)] 這個是起始位址
                因為我們的測試矩陣輸入是 6 個 limbs，所以起始位置是 out[6 * (c * bs_mat_cols + k)]，接著把原始矩陣的 limb 0 ~ 4 複製到 out，這時會多出 limb 5 沒東西，但沒差
                out[6 * (c * bs_mat_cols + k) + 0] = in[5 * (c * bs_mat_cols + k) + 0] = 第 0 個 limb 複製到新矩陣
                out[6 * (c * bs_mat_cols + k) + 1] = in[5 * (c * bs_mat_cols + k) + 1] = 第 1 個 limb 複製到新矩陣
                ...
                out[6*(c * bs_mat_cols + k) + i] = in[5*(c * bs_mat_cols + k) + i] for i = 0 ~ 4
                */ 
            }
        }
    }
}

static int mem_compare(uint64_t *acc_ref, uint64_t *acc){
    for (int c = 0; c < MAT_ROWS; c++) {
        for (int k = 0; k < BS_MAT_COLS; k += 1) {
            for(int i = 0; i < M_VEC_LIMBS; i++){
                if (acc_ref[(c * BS_MAT_COLS + k) * M_VEC_LIMBS + i] != acc[(c * BS_MAT_COLS + k) * (M_VEC_LIMBS + 1) + i]) {
                    printf("\n");
                    printf("c = %d, k = %d, i = %d\n", c, k, i);
                    printf("acc_ref[%d] = %llu\n", (c * BS_MAT_COLS + k) * M_VEC_LIMBS + i, acc_ref[(c * BS_MAT_COLS + k) * M_VEC_LIMBS + i]);
                    printf("acc_mve[%d] = %llu\n", (c * BS_MAT_COLS + k) * (M_VEC_LIMBS + 1) + i, acc[(c * BS_MAT_COLS + k) * (M_VEC_LIMBS + 1) + i]);
                    return 1;
                }    
            }
        }
    }

    return 0;
}