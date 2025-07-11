#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "../src/api.h"

#define TEST_GENKEY 10
#define TEST_RUN 100

ITCM_FN int main(void) {
    printf("%s\n", OV_ALGNAME );
    printf("sk size: %d\n", CRYPTO_SECRETKEYBYTES );
    printf("pk size: %d\n", CRYPTO_PUBLICKEYBYTES );
    printf("signature overhead: %d\n\n", CRYPTO_BYTES );

    // mlen : the length of the message
    uint64_t mlen = 256; // 53 or 256 ，53 只是用來測試不同長度的訊息也可以運作，通常用在 Benchmark。選一個不是 2 的次方、也不是某個長度倍數的數字
    uint8_t sm[mlen + CRYPTO_BYTES]; // Signed message
    uint8_t m[mlen];
    for (unsigned i = 0; i < mlen; i++) {
        m[i] = i;
    }
    
    uint64_t smlen, cycles;
    uint8_t pk[CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[CRYPTO_SECRETKEYBYTES]; // 在 M85 可能不支援 malloc，所以不要用
    int fail = 0;

    printf("===========  test crypto_sign_keypair(), crypto_sign(), and crypto_sign_open()  ================\n\n");
    for (unsigned i = 0; i < TEST_RUN; i++) {
        if ( i < TEST_GENKEY ) {
            int r0;
            bench_cycles2(crypto_sign_keypair( pk, sk), cycles, r0);
            if ( 0 != r0 ) {
                printf("generating key return %d.\n", r0);
                fail = -1;
                break;
            }
        }

        for (unsigned j = 3; j < mlen; j++) {
            m[j] = (i * j) & 0xff;
        }

        int r1, r2;
        bench_cycles2(crypto_sign( sm, &smlen, m, mlen, sk ), cycles, r1);
        if ( 0 != r1 ) {
            printf("crypto_sign() return %d.\n", r1);
            fail = -1;
            break;
        }

        bench_cycles2(crypto_sign_open( m, &mlen, sm, smlen, pk ), cycles, r2);
        if ( 0 != r2 ) {
            printf("crypto_sign_open() return %d.\n", r2);
            fail = -1;
            break;
        }
    }

    if (fail) {
        printf("Tests failed.\n");
    } 
    else {
        printf("all (%d,%d) tests passed.\n\n", TEST_RUN, TEST_GENKEY );
    }
    
    return 0;
}