#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/api.h"
#include "utils.h"

#define TEST_GENKEY 10
#define TEST_RUN 100

ITCM_FN int main(void) {
    printf("%s\n", OV_ALGNAME );
    printf("sk size: %d\n", CRYPTO_SECRETKEYBYTES );
    printf("pk size: %d\n", CRYPTO_PUBLICKEYBYTES );
    printf("signature overhead: %d\n\n", CRYPTO_BYTES );
    int fail = 0;

    unsigned char sm[256 + CRYPTO_BYTES];
    unsigned char m[256];
    for (unsigned i = 0; i < 256; i++) {
        m[i] = i;
    }
    unsigned long long mlen = 256; // 53 or 256 
    unsigned long long smlen, cycles;

    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];

    printf("=========== UOV-Ip: crypto_sign_keypair(), crypto_sign(), and crypto_sign_open() ================\n\n");
    for (unsigned i = 0; i < TEST_RUN; i++) {
        if ( i < TEST_GENKEY ) {
            int r0;
            bench_cycles2(crypto_sign_keypair( pk, sk), cycles, r0); // mve
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
        bench_cycles2(crypto_sign( sm, &smlen, m, mlen, sk ), cycles, r2); // mve
        if ( 0 != r1 ) {
            printf("crypto_sign() return %d.\n", r1);
            fail = -1;
            break;
        }
        //sm + *mlen = 352 - 256
        r2 = crypto_sign_open( m, &mlen, sm, smlen, pk );
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