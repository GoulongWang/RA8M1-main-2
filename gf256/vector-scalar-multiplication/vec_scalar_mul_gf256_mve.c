#include <arm_mve.h>
#include <arm_acle.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../randombytes.h"

#define TEST_RUN 100

static uint32_t gf256v_mul_u32(uint32_t a, uint8_t b);
void vec_scalar_mul_gf256(uint32_t *out_vec, uint32_t *in_vec, uint8_t scalar);

int main (void)
{
    uint32_t input[4]; // 128 bits
    uint8_t scalar;
    uint32_t output_mve[4];
    uint32_t output_ref[4];

    for (int j = 1; j <= TEST_RUN; j++) {
        randombytes((uint8_t*) input, sizeof input * 4);
        randombytes(&scalar, sizeof(scalar));
        memset(output_mve, 0, sizeof(output_mve));
        memset(output_ref, 0, sizeof(output_ref));
      
        printf("\nTest %d:\n", j);
        printf("Input = [ ");
        for (int i = 0; i < 4; i++) {
            uint32_t w = input[i];
            printf("%02x %02x %02x %02x ",
                    (w >> 24) & 0xFF,
                    (w >> 16) & 0xFF,
                    (w >>  8) & 0xFF,
                     w        & 0xFF);
        }
        printf("]\n");
        printf("Scalar = %x\n", scalar);

        printf("output_ref = [ ");
        for (int i = 0; i < 4; i++) {
            output_ref[i] = gf256v_mul_u32(input[i], scalar);

            uint32_t w = output_ref[i];
            printf("%02x %02x %02x %02x ",
                    (w >> 24) & 0xFF,
                    (w >> 16) & 0xFF,
                    (w >>  8) & 0xFF,
                     w        & 0xFF);
        }
        printf("]\n");

        vec_scalar_mul_gf256(output_mve, input, scalar);

        if(memcmp(&output_mve, &output_ref, sizeof output_ref)){
            break;
            printf("ERROR!\n");
        } else {
            printf("OK!\n");
        }
    }
    return( 0 );
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