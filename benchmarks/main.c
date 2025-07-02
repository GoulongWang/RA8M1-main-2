#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "../src/api.h"

#define TEST_GENKEY 50
#define TEST_RUN 500

ITCM_FN int main(void) {
    printf("%s\n", OV_ALGNAME );
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
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES]; // 在 M85 可能不支援 malloc，所以不要用
    uint64_t cycles;
    int ret = 0;

    printf("===========  test crypto_sign_keypair(), crypto_sign(), and crypto_sign_open()  ================\n\n");
    for (unsigned i = 0; i < TEST_RUN; i++) {
        // 為什麼 GENKEY 只要測試 50 次？
        if ( i < TEST_GENKEY ) {
            int r0;
            bench_cycles2(crypto_sign_keypair( pk, sk), cycles, r0);
            if ( 0 != r0 ) {
                printf("generating key return %d.\n", r0);
                ret = -1;
                break;
            }
        }

        for (unsigned j = 3; j < 256; j++) {
            m[j] = (i * j) & 0xff;
        }
        int r1, r2;
        bench_cycles2(crypto_sign( sm, &smlen, m, mlen, sk ), cycles, r1);
        if ( 0 != r1 ) {
            printf("crypto_sign() return %d.\n", r1);
            ret = -1;
            break;
        }
        bench_cycles2(crypto_sign_open( m, &mlen, sm, smlen, pk ), cycles, r2);
        if ( 0 != r2 ) {
            printf("crypto_sign_open() return %d.\n", r2);
            ret = -1;
            break;
        }
    }

    printf("all (%d,%d) tests passed.\n\n", TEST_RUN, TEST_GENKEY );
     
    printf("===========  test crypto_sign_keypair(), crypto_sign_signature(), and crypto_sign_verify()  ================\n\n");
    mlen = 53;
    unsigned long long siglen;
    unsigned char sig[CRYPTO_BYTES];
    for (unsigned i = 0; i < TEST_RUN; i++) {
        int rc;
        bench_cycles2(crypto_sign_keypair( pk, sk), cycles, rc);
        if ( 0 != rc ) {
            printf("generating key returned %d.\n", rc);
            ret = -1;
        }


        for (unsigned j = 3; j < 53; j++) {
            m[j] = (i * j) & 0xff;
        }

        bench_cycles2(crypto_sign_signature( sig, &siglen, m, mlen, sk ), cycles, rc);

        if ( 0 != rc ) {
            printf("crypto_sign_signature() returned %d.\n", rc);
            ret = -1;
        }

        bench_cycles2(crypto_sign_verify( sig, siglen,  m, mlen, pk ), cycles, rc);
        if ( 0 != rc ) {
            printf("crypto_sign_verify() return %d.\n", rc);
            ret = -1;
        }

    }

    printf("all (%d) tests passed.\n\n", TEST_RUN );
    return ret;
}