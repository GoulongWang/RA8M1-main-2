#include <arm_mve.h>
#include <arm_acle.h>
#include <stdio.h>
#include <string.h>
#include "../../randombytes.h"

#define TEST_RUN 100

void mve_mul_gf16( uint8_t *dst, uint8_t *src, uint8_t c);

// gf16 := gf2[x]/(x^4+x+1)
static inline uint8_t gf16_mul(uint8_t a, uint8_t b) {
  uint8_t r8 = (a & 1) * b;
  r8 ^= (a & 2) * b;
  r8 ^= (a & 4) * b;
  r8 ^= (a & 8) * b;

  // reduction
  uint8_t r4 = r8 ^ (((r8 >> 4) & 5) * 3); // x^4 = x+1 , x^6 = x^3 + x^2
  r4 ^= (((r8 >> 5) & 1) * 6);             // x^5 = x^2 + x
  return (r4 & 0xf);
}

static void gf16v_mul_ref(uint8_t out[16], uint8_t in[16], uint8_t scalar){
  uint8_t highnib_in[16], temp[16];
  for(int i=0; i<16; i++){
    highnib_in[i] = in[i] >> 4;
    temp[i] = gf16_mul(highnib_in[i], scalar); 
    highnib_in[i] = temp[i] << 4;
    out[i] = gf16_mul(in[i], scalar);
    out[i] = out[i] ^ highnib_in[i];
  }
}

int main (void)
{
    #if defined(EXEC_TB) && defined(__ARM_FEATURE_MVE)
    EXECTB_Init();
    enableCde();
    initTick();
    #endif

    uint8_t input[16];
    uint8_t output[16];
    uint8_t output_ref[16];
    uint8_t scalar;
    
    for (int j = 1; j <= TEST_RUN; j++) {
      randombytes(input, sizeof input);
      randombytes(&scalar, 1);
      scalar = scalar >> 4;
      
      mve_mul_gf16(output,input, scalar);
      gf16v_mul_ref(output_ref, input, scalar);

      printf("\nTest %d:\n", j);
      printf("Constant = %d\n", scalar);
      int i;
      printf("Reference = [");
      for(i = 0; i < 15; i++){
	      printf("%02x, ", output_ref[i]);    
      }
      printf("%02x]\n", output_ref[15]);

      printf("MVE = [");
      for(i = 0; i < 15; i++){
	      printf("%02x, ", output[i]);
      }
      printf("%02x]\n", output[15]);

      if(memcmp(output, output_ref, sizeof output)){
        break;
        printf("ERROR!\n");
      } else {
        printf("OK!\n");
      }
    }
    return( 0 );
}