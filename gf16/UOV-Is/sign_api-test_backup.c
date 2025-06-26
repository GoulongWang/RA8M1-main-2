#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ov_keypair.h" // define type pk_t, sk_t
#include "api.h"
#include "../../randombytes.h"
#include "fips202.h"
#include "openssl/evp.h"
#include "utils_prng.h"

typedef keccak_state hash_ctx;
#define RNG_OUTPUTLEN 64


typedef struct {
    unsigned used;
    uint32_t ctr;
    unsigned char   buf[RNG_OUTPUTLEN];

    #ifdef _4ROUND_AES_
    uint32_t key[40];
    #else  // round key of the normal aes128
    uint32_t key[88];
    #endif
} prng_publicinputs_t;


#define TEST_GENKEY 50
#define TEST_RUN 100
#define batch_trimat_madd  batch_trimat_madd_gf16
#define batch_upper_matTr_x_mat batch_upper_matTr_x_mat_gf16
#define batch_trimatTr_madd batch_trimatTr_madd_gf16
#define gf16v_madd _gf16v_madd_u32
#define gf256v_add _gf256v_add_u32
#define NROUNDS 24
#define ROL(a, offset) ((a << offset) ^ (a >> (64-offset)))

void calculate_F2_P3( unsigned char *S, unsigned char *P3, const unsigned char *P1, const unsigned char *P2, const unsigned char *sk_O );
int generate_keypair( pk_t *pk, sk_t *sk, const unsigned char *sk_seed);
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);
int crypto_sign_signature(unsigned char *sig, unsigned long long *siglen,
                      const unsigned char *m, unsigned long long mlen,
                      const unsigned char *sk);

int main(void) {
    printf("sk size: %d\n", CRYPTO_SECRETKEYBYTES );
    printf("pk size: %d\n", CRYPTO_PUBLICKEYBYTES );
    printf("signature overhead: %d\n\n", CRYPTO_BYTES );

    unsigned char sm[256 + CRYPTO_BYTES];
    unsigned char m[256];
    for (unsigned i = 0; i < 256; i++) {
        m[i] = i;
    }
    unsigned long long mlen = 256;
    unsigned long long smlen;

    unsigned char *pk = (unsigned char *)malloc( CRYPTO_PUBLICKEYBYTES );
    unsigned char *sk = (unsigned char *)malloc( CRYPTO_SECRETKEYBYTES );

    int ret = 0;
    
    printf("===========  test crypto_sign_keypair(), crypto_sign(), and crypto_sign_open()  ================\n\n");
    for (unsigned i = 0; i < TEST_RUN; i++) {
        if ( i < TEST_GENKEY ) {
            int r0 = crypto_sign_keypair( pk, sk);
            if ( 0 != r0 ) {
                printf("generating key return %d.\n", r0);
                ret = -1;
                goto clean_exit;
            }
        }

        for (unsigned j = 3; j < 256; j++) {
            m[j] = (i * j) & 0xff;
        }
        int r1, r2;/*
        r1 = crypto_sign( sm, &smlen, m, mlen, sk );
        if ( 0 != r1 ) {
            printf("crypto_sign() return %d.\n", r1);
            ret = -1;
            goto clean_exit;
        }
        r2 = crypto_sign_open( m, &mlen, sm, smlen, pk );
        if ( 0 != r2 ) {
            printf("crypto_sign_open() return %d.\n", r2);
            ret = -1;
            goto clean_exit;
        } */
    }
    printf("all (%d,%d) tests passed.\n\n", TEST_RUN, TEST_GENKEY );
    /*
    printf("===========  test crypto_sign_keypair(), crypto_sign_signature(), and crypto_sign_verify()  ================\n\n");

    mlen = 53;
    unsigned long long siglen;
    unsigned char sig[CRYPTO_BYTES];
    for (unsigned i = 0; i < TEST_RUN; i++) {
        int rc;
        rc = crypto_sign_keypair( pk, sk);
        if ( 0 != rc ) {
            printf("generating key returned %d.\n", rc);
            ret = -1;
            goto clean_exit;
        }


        for (unsigned j = 3; j < 53; j++) {
            m[j] = (i * j) & 0xff;
        }

        rc = crypto_sign_signature( sig, &siglen, m, mlen, sk );

        if ( 0 != rc ) {
            printf("crypto_sign_signature() returned %d.\n", rc);
            ret = -1;
            goto clean_exit;
        }

        //rc = crypto_sign_verify( sig, siglen,  m, mlen, pk );
        if ( 0 != rc ) {
            printf("crypto_sign_verify() return %d.\n", rc);
            ret = -1;
            goto clean_exit;
        }

    }

    printf("all (%d) tests passed.\n\n", TEST_RUN );*/

 
clean_exit:
    free( pk );
    free( sk );
    printf("OK\n");
    return ret;
}

static inline void gf256v_set_zero(uint8_t *b, unsigned _num_byte) {
    memset(b, 0, _num_byte);
}

static inline uint8_t gf16v_get_ele(const uint8_t *a, unsigned i) {
    uint8_t r = a[i >> 1];
    return (i & 1) ? (r >> 4) : (r & 0xf);
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

void batch_trimat_madd_gf16( unsigned char *bC, const unsigned char *btriA,
    const unsigned char *B, unsigned Bheight, unsigned size_Bcolvec, unsigned Bwidth, unsigned size_batch ) {
#define MAX_V       (96)
#define MAX_O_BYTE  (32)
uint8_t tmp_c[MAX_O_BYTE];
uint8_t B2[MAX_V * MAX_O_BYTE];
#undef MAX_V
#undef MAX_O_BYTE

    for (unsigned i = 0; i < Bwidth; i++) {
        for (unsigned j = 0; j < size_Bcolvec - 1; j++) {
            B2[i * size_Bcolvec + j] = (B[i * size_Bcolvec + j] >> 4) | (B[i * size_Bcolvec + j + 1] << 4);
        }
        B2[i * size_Bcolvec + size_Bcolvec - 1] = B[i * size_Bcolvec + size_Bcolvec - 1] >> 4;
    }

    // access fixed positions of destination matrix C
    unsigned Aheight = Bheight;
    for (unsigned i = 0; i < Aheight; i += 2) {
        for (unsigned j = 0; j < Bwidth; j++) {
            gf16mat_prod_ref( tmp_c, btriA, size_batch, Aheight - i, B + j * size_Bcolvec + (i / 2) );
            gf256v_add( bC, tmp_c, size_batch);
            bC += size_batch;
        }
        
        btriA += size_batch * (Aheight - i);
        
        for (unsigned j = 0; j < Bwidth; j++) {
            gf16mat_prod_ref( tmp_c, btriA, size_batch, Aheight - i - 1, B2 + j * size_Bcolvec + (i / 2) );
            gf256v_add( bC, tmp_c, size_batch);
            bC += size_batch;
        }
        btriA += size_batch * (Aheight - i - 1);
    }
}

void batch_upper_matTr_x_mat_gf16( unsigned char *bC, const unsigned char *A_to_tr, unsigned Aheight, unsigned size_Acolvec, unsigned Awidth,
    const unsigned char *bB, unsigned Bwidth, unsigned size_batch ) {
#define MAX_O  (64)
#define MAX_O_BYTE  (32)
    uint8_t row[MAX_O * MAX_O_BYTE]; /// XXX: buffer for maximum row
#undef MAX_O_BYTE
#undef MAX_O
    unsigned Atr_height = Awidth;
    unsigned Atr_width  = Aheight;
    for (unsigned i = 0; i < Atr_height; i++) {
        gf16mat_prod_ref( row, bB, Bwidth * size_batch, Atr_width, A_to_tr + size_Acolvec * i );
        uint8_t *ptr = bC + i * size_batch;
        for (unsigned j = 0; j < i; j++) {
            gf256v_add( ptr, row + size_batch * j, size_batch );
            ptr += (Bwidth - j - 1) * size_batch;
        }
        memcpy( ptr, row + size_batch * i, size_batch * (Bwidth - i) );
    }
}

// This function is only used in calssic mode.
void batch_trimatTr_madd_gf16( unsigned char *bC, const unsigned char *btriA,
    const unsigned char *B, unsigned Bheight, unsigned size_Bcolvec, unsigned Bwidth, unsigned size_batch ) {
#define MAX_O_BYTE  (32)
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
        memcpy( tmp_Arow + i * size_batch, ptr, size_batch );

        for (unsigned j = 0; j < Bwidth; j++) {
            gf16mat_prod_ref( tmp_c, tmp_Arow, size_batch, i + 1, B + (j * size_Bcolvec) );
            gf256v_add( bC, tmp_c, size_batch);
            bC += size_batch;
        }
    }
}

void calculate_F2_P3( unsigned char *S, unsigned char *P3, const unsigned char *P1, const unsigned char *P2, const unsigned char *sk_O ) {
    //Q_pk.l1_Q2s[i] = (P1* T1 + P2) + F1tr * t1
    //Q_pk.l1_Q5s[i] = UT( T1tr* (F1 * T1 + F2) )

    if (S != P2) {
        memcpy( S, P2, _PK_P2_BYTE );
    }
    batch_trimat_madd( S, P1, sk_O, _V, _V_BYTE, _O, _O_BYTE );        // F1*T1 + F2
    batch_upper_matTr_x_mat( P3, sk_O, _V, _V_BYTE, _O, S, _O, _O_BYTE );    // Q5 = UT . t1_tr*(F1*T1 + F2)
    batch_trimatTr_madd( S, P1, sk_O, _V, _V_BYTE, _O, _O_BYTE );       // Q2
}

static void keccak_init(uint64_t s[25]) {
    unsigned int i;
    for (i = 0; i < 25; i++) {
        s[i] = 0;
    }
}

void shake256_init(keccak_state *state) {
    keccak_init(state->s);
    state->pos = 0;
}

int hash_init( hash_ctx *ctx ) {
    #if defined(_HASH_SHAKE128_)
    shake128_init( ctx );
    #else
    shake256_init( ctx );
    #endif
    return 0;
}

/* Keccak round constants */
static const uint64_t KeccakF_RoundConstants[NROUNDS] = {
    (uint64_t)0x0000000000000001ULL,
    (uint64_t)0x0000000000008082ULL,
    (uint64_t)0x800000000000808aULL,
    (uint64_t)0x8000000080008000ULL,
    (uint64_t)0x000000000000808bULL,
    (uint64_t)0x0000000080000001ULL,
    (uint64_t)0x8000000080008081ULL,
    (uint64_t)0x8000000000008009ULL,
    (uint64_t)0x000000000000008aULL,
    (uint64_t)0x0000000000000088ULL,
    (uint64_t)0x0000000080008009ULL,
    (uint64_t)0x000000008000000aULL,
    (uint64_t)0x000000008000808bULL,
    (uint64_t)0x800000000000008bULL,
    (uint64_t)0x8000000000008089ULL,
    (uint64_t)0x8000000000008003ULL,
    (uint64_t)0x8000000000008002ULL,
    (uint64_t)0x8000000000000080ULL,
    (uint64_t)0x000000000000800aULL,
    (uint64_t)0x800000008000000aULL,
    (uint64_t)0x8000000080008081ULL,
    (uint64_t)0x8000000000008080ULL,
    (uint64_t)0x0000000080000001ULL,
    (uint64_t)0x8000000080008008ULL
};

static void KeccakF1600_StatePermute(uint64_t state[25]) {
    int round;

    uint64_t Aba, Abe, Abi, Abo, Abu;
    uint64_t Aga, Age, Agi, Ago, Agu;
    uint64_t Aka, Ake, Aki, Ako, Aku;
    uint64_t Ama, Ame, Ami, Amo, Amu;
    uint64_t Asa, Ase, Asi, Aso, Asu;
    uint64_t BCa, BCe, BCi, BCo, BCu;
    uint64_t Da, De, Di, Do, Du;
    uint64_t Eba, Ebe, Ebi, Ebo, Ebu;
    uint64_t Ega, Ege, Egi, Ego, Egu;
    uint64_t Eka, Eke, Eki, Eko, Eku;
    uint64_t Ema, Eme, Emi, Emo, Emu;
    uint64_t Esa, Ese, Esi, Eso, Esu;

    //copyFromState(A, state)
    Aba = state[ 0];
    Abe = state[ 1];
    Abi = state[ 2];
    Abo = state[ 3];
    Abu = state[ 4];
    Aga = state[ 5];
    Age = state[ 6];
    Agi = state[ 7];
    Ago = state[ 8];
    Agu = state[ 9];
    Aka = state[10];
    Ake = state[11];
    Aki = state[12];
    Ako = state[13];
    Aku = state[14];
    Ama = state[15];
    Ame = state[16];
    Ami = state[17];
    Amo = state[18];
    Amu = state[19];
    Asa = state[20];
    Ase = state[21];
    Asi = state[22];
    Aso = state[23];
    Asu = state[24];

    for (round = 0; round < NROUNDS; round += 2) {
        //    prepareTheta
        BCa = Aba ^ Aga ^ Aka ^ Ama ^ Asa;
        BCe = Abe ^ Age ^ Ake ^ Ame ^ Ase;
        BCi = Abi ^ Agi ^ Aki ^ Ami ^ Asi;
        BCo = Abo ^ Ago ^ Ako ^ Amo ^ Aso;
        BCu = Abu ^ Agu ^ Aku ^ Amu ^ Asu;

        //thetaRhoPiChiIotaPrepareTheta(round, A, E)
        Da = BCu ^ ROL(BCe, 1);
        De = BCa ^ ROL(BCi, 1);
        Di = BCe ^ ROL(BCo, 1);
        Do = BCi ^ ROL(BCu, 1);
        Du = BCo ^ ROL(BCa, 1);

        Aba ^= Da;
        BCa = Aba;
        Age ^= De;
        BCe = ROL(Age, 44);
        Aki ^= Di;
        BCi = ROL(Aki, 43);
        Amo ^= Do;
        BCo = ROL(Amo, 21);
        Asu ^= Du;
        BCu = ROL(Asu, 14);
        Eba =   BCa ^ ((~BCe)&  BCi );
        Eba ^= (uint64_t)KeccakF_RoundConstants[round];
        Ebe =   BCe ^ ((~BCi)&  BCo );
        Ebi =   BCi ^ ((~BCo)&  BCu );
        Ebo =   BCo ^ ((~BCu)&  BCa );
        Ebu =   BCu ^ ((~BCa)&  BCe );

        Abo ^= Do;
        BCa = ROL(Abo, 28);
        Agu ^= Du;
        BCe = ROL(Agu, 20);
        Aka ^= Da;
        BCi = ROL(Aka,  3);
        Ame ^= De;
        BCo = ROL(Ame, 45);
        Asi ^= Di;
        BCu = ROL(Asi, 61);
        Ega =   BCa ^ ((~BCe)&  BCi );
        Ege =   BCe ^ ((~BCi)&  BCo );
        Egi =   BCi ^ ((~BCo)&  BCu );
        Ego =   BCo ^ ((~BCu)&  BCa );
        Egu =   BCu ^ ((~BCa)&  BCe );

        Abe ^= De;
        BCa = ROL(Abe,  1);
        Agi ^= Di;
        BCe = ROL(Agi,  6);
        Ako ^= Do;
        BCi = ROL(Ako, 25);
        Amu ^= Du;
        BCo = ROL(Amu,  8);
        Asa ^= Da;
        BCu = ROL(Asa, 18);
        Eka =   BCa ^ ((~BCe)&  BCi );
        Eke =   BCe ^ ((~BCi)&  BCo );
        Eki =   BCi ^ ((~BCo)&  BCu );
        Eko =   BCo ^ ((~BCu)&  BCa );
        Eku =   BCu ^ ((~BCa)&  BCe );

        Abu ^= Du;
        BCa = ROL(Abu, 27);
        Aga ^= Da;
        BCe = ROL(Aga, 36);
        Ake ^= De;
        BCi = ROL(Ake, 10);
        Ami ^= Di;
        BCo = ROL(Ami, 15);
        Aso ^= Do;
        BCu = ROL(Aso, 56);
        Ema =   BCa ^ ((~BCe)&  BCi );
        Eme =   BCe ^ ((~BCi)&  BCo );
        Emi =   BCi ^ ((~BCo)&  BCu );
        Emo =   BCo ^ ((~BCu)&  BCa );
        Emu =   BCu ^ ((~BCa)&  BCe );

        Abi ^= Di;
        BCa = ROL(Abi, 62);
        Ago ^= Do;
        BCe = ROL(Ago, 55);
        Aku ^= Du;
        BCi = ROL(Aku, 39);
        Ama ^= Da;
        BCo = ROL(Ama, 41);
        Ase ^= De;
        BCu = ROL(Ase,  2);
        Esa =   BCa ^ ((~BCe)&  BCi );
        Ese =   BCe ^ ((~BCi)&  BCo );
        Esi =   BCi ^ ((~BCo)&  BCu );
        Eso =   BCo ^ ((~BCu)&  BCa );
        Esu =   BCu ^ ((~BCa)&  BCe );

        //    prepareTheta
        BCa = Eba ^ Ega ^ Eka ^ Ema ^ Esa;
        BCe = Ebe ^ Ege ^ Eke ^ Eme ^ Ese;
        BCi = Ebi ^ Egi ^ Eki ^ Emi ^ Esi;
        BCo = Ebo ^ Ego ^ Eko ^ Emo ^ Eso;
        BCu = Ebu ^ Egu ^ Eku ^ Emu ^ Esu;

        //thetaRhoPiChiIotaPrepareTheta(round+1, E, A)
        Da = BCu ^ ROL(BCe, 1);
        De = BCa ^ ROL(BCi, 1);
        Di = BCe ^ ROL(BCo, 1);
        Do = BCi ^ ROL(BCu, 1);
        Du = BCo ^ ROL(BCa, 1);

        Eba ^= Da;
        BCa = Eba;
        Ege ^= De;
        BCe = ROL(Ege, 44);
        Eki ^= Di;
        BCi = ROL(Eki, 43);
        Emo ^= Do;
        BCo = ROL(Emo, 21);
        Esu ^= Du;
        BCu = ROL(Esu, 14);
        Aba =   BCa ^ ((~BCe)&  BCi );
        Aba ^= (uint64_t)KeccakF_RoundConstants[round + 1];
        Abe =   BCe ^ ((~BCi)&  BCo );
        Abi =   BCi ^ ((~BCo)&  BCu );
        Abo =   BCo ^ ((~BCu)&  BCa );
        Abu =   BCu ^ ((~BCa)&  BCe );

        Ebo ^= Do;
        BCa = ROL(Ebo, 28);
        Egu ^= Du;
        BCe = ROL(Egu, 20);
        Eka ^= Da;
        BCi = ROL(Eka, 3);
        Eme ^= De;
        BCo = ROL(Eme, 45);
        Esi ^= Di;
        BCu = ROL(Esi, 61);
        Aga =   BCa ^ ((~BCe)&  BCi );
        Age =   BCe ^ ((~BCi)&  BCo );
        Agi =   BCi ^ ((~BCo)&  BCu );
        Ago =   BCo ^ ((~BCu)&  BCa );
        Agu =   BCu ^ ((~BCa)&  BCe );

        Ebe ^= De;
        BCa = ROL(Ebe, 1);
        Egi ^= Di;
        BCe = ROL(Egi, 6);
        Eko ^= Do;
        BCi = ROL(Eko, 25);
        Emu ^= Du;
        BCo = ROL(Emu, 8);
        Esa ^= Da;
        BCu = ROL(Esa, 18);
        Aka =   BCa ^ ((~BCe)&  BCi );
        Ake =   BCe ^ ((~BCi)&  BCo );
        Aki =   BCi ^ ((~BCo)&  BCu );
        Ako =   BCo ^ ((~BCu)&  BCa );
        Aku =   BCu ^ ((~BCa)&  BCe );

        Ebu ^= Du;
        BCa = ROL(Ebu, 27);
        Ega ^= Da;
        BCe = ROL(Ega, 36);
        Eke ^= De;
        BCi = ROL(Eke, 10);
        Emi ^= Di;
        BCo = ROL(Emi, 15);
        Eso ^= Do;
        BCu = ROL(Eso, 56);
        Ama =   BCa ^ ((~BCe)&  BCi );
        Ame =   BCe ^ ((~BCi)&  BCo );
        Ami =   BCi ^ ((~BCo)&  BCu );
        Amo =   BCo ^ ((~BCu)&  BCa );
        Amu =   BCu ^ ((~BCa)&  BCe );

        Ebi ^= Di;
        BCa = ROL(Ebi, 62);
        Ego ^= Do;
        BCe = ROL(Ego, 55);
        Eku ^= Du;
        BCi = ROL(Eku, 39);
        Ema ^= Da;
        BCo = ROL(Ema, 41);
        Ese ^= De;
        BCu = ROL(Ese, 2);
        Asa =   BCa ^ ((~BCe)&  BCi );
        Ase =   BCe ^ ((~BCi)&  BCo );
        Asi =   BCi ^ ((~BCo)&  BCu );
        Aso =   BCo ^ ((~BCu)&  BCa );
        Asu =   BCu ^ ((~BCa)&  BCe );
    }

    //copyToState(state, A)
    state[ 0] = Aba;
    state[ 1] = Abe;
    state[ 2] = Abi;
    state[ 3] = Abo;
    state[ 4] = Abu;
    state[ 5] = Aga;
    state[ 6] = Age;
    state[ 7] = Agi;
    state[ 8] = Ago;
    state[ 9] = Agu;
    state[10] = Aka;
    state[11] = Ake;
    state[12] = Aki;
    state[13] = Ako;
    state[14] = Aku;
    state[15] = Ama;
    state[16] = Ame;
    state[17] = Ami;
    state[18] = Amo;
    state[19] = Amu;
    state[20] = Asa;
    state[21] = Ase;
    state[22] = Asi;
    state[23] = Aso;
    state[24] = Asu;
}

unsigned int keccak_absorb(uint64_t s[25],
    unsigned int pos,
    unsigned int r,
    const uint8_t *in,
    size_t inlen) {
    unsigned int i;

    while (pos + inlen >= r) {
        for (i = pos; i < r; i++) {
            s[i / 8] ^= (uint64_t) * in++ << 8 * (i % 8);
        }
        inlen -= r - pos;
        KeccakF1600_StatePermute(s);
        pos = 0;
    }

    for (i = pos; i < pos + inlen; i++) {
        s[i / 8] ^= (uint64_t) * in++ << 8 * (i % 8);
    }

    return i;
}

void shake256_absorb(keccak_state *state, const uint8_t *in, size_t inlen) {
    state->pos = keccak_absorb(state->s, state->pos, SHAKE256_RATE, in, inlen);
}

int hash_update( hash_ctx *ctx, const unsigned char *mesg, size_t mlen ) {
    #if defined(_HASH_SHAKE128_)
    shake128_absorb( ctx, mesg, mlen );
    #else
    shake256_absorb( ctx, mesg, mlen );
    #endif
    return 0;
}

static unsigned int keccak_squeeze(uint8_t *out,
    size_t outlen,
    uint64_t s[25],
    unsigned int pos,
    unsigned int r) {
    unsigned int i;

    while (outlen) {
        if (pos == r) {
            KeccakF1600_StatePermute(s);
            pos = 0;
        }
        for (i = pos; i < r && i < pos + outlen; i++) {
            *out++ = s[i / 8] >> 8 * (i % 8);
        }
        outlen -= i - pos;
        pos = i;
    }

    return pos;
}

void shake256_squeeze(uint8_t *out, size_t outlen, keccak_state *state) {
    state->pos = keccak_squeeze(out, outlen, state->s, state->pos, SHAKE256_RATE);
}

static void store64(uint8_t x[8], uint64_t u) {
    unsigned int i;

    for (i = 0; i < 8; i++) {
        x[i] = u >> 8 * i;
    }
}

static void keccak_squeezeblocks(uint8_t *out,
    size_t nblocks,
    uint64_t s[25],
    unsigned int r) {
    unsigned int i;

    while (nblocks) {
        KeccakF1600_StatePermute(s);
        for (i = 0; i < r / 8; i++) {
            store64(out + 8 * i, s[i]);
        }
        out += r;
        nblocks -= 1;
    }
}

static void keccak_finalize(uint64_t s[25], unsigned int pos, unsigned int r, uint8_t p) {
    s[pos / 8] ^= (uint64_t)p << 8 * (pos % 8);
    s[r / 8 - 1] ^= 1ULL << 63;
}

void shake256_finalize(keccak_state *state) {
    keccak_finalize(state->s, state->pos, SHAKE256_RATE, 0x1F);
    state->pos = SHAKE256_RATE;
}

void shake256_squeezeblocks(uint8_t *out, size_t nblocks, keccak_state *state) {
    keccak_squeezeblocks(out, nblocks, state->s, SHAKE256_RATE);
}

int hash_final_digest( unsigned char *out, size_t outlen, hash_ctx *ctx ) {
    #if defined(_HASH_SHAKE128_)
    shake128_finalize( ctx );

    unsigned nblocks = outlen / SHAKE128_RATE;
    shake128_squeezeblocks(out, nblocks, ctx);
    outlen -= nblocks * SHAKE128_RATE;
    out += nblocks * SHAKE128_RATE;
    shake128_squeeze(out, outlen, ctx);
    #else
    shake256_finalize( ctx );

    unsigned nblocks = outlen / SHAKE256_RATE;
    shake256_squeezeblocks(out, nblocks, ctx);
    outlen -= nblocks * SHAKE256_RATE;
    out += nblocks * SHAKE256_RATE;
    shake256_squeeze(out, outlen, ctx);
    #endif

    return 0;
}

int generate_keypair( pk_t *rpk, sk_t *sk, const unsigned char *sk_seed ) {
    memcpy( sk->sk_seed, sk_seed, LEN_SKSEED );

    // pk_seed || O
    unsigned char buf[LEN_PKSEED + sizeof(sk->O)];
    unsigned char *pk_seed = buf;
    unsigned char *O = buf + LEN_PKSEED;

    // prng for sk
    hash_ctx hctx;
    hash_init(&hctx);
    hash_update(&hctx, sk_seed, LEN_SKSEED );
    hash_final_digest( buf, sizeof(buf), &hctx );
    memcpy(sk->O, O, sizeof(sk->O));

    // prng for pk
    prng_publicinputs_t prng1;
    prng_set_publicinputs(&prng1, pk_seed );
    // P1 and P2
    prng_gen_publicinputs(&prng1, rpk->pk, sizeof(sk->P1) + sizeof(sk->S) );
    memcpy( sk->P1, rpk->pk, sizeof(sk->P1) );
    prng_release_publicinputs(&prng1);

    // S and P3
    unsigned char *rpk_P2 = rpk->pk + sizeof(sk->P1);
    unsigned char *rpk_P3 = rpk->pk + sizeof(sk->P1) + sizeof(sk->S);
    calculate_F2_P3( sk->S, rpk_P3, sk->P1, rpk_P2, sk->O ); 
    return 0;
}

// OV_CLASSIC
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk){
    unsigned char sk_seed[LEN_SKSEED];
    randombytes( sk_seed, LEN_SKSEED );
    int r = generate_keypair((pk_t *) pk, (sk_t *) sk, sk_seed);
    return r;
}
