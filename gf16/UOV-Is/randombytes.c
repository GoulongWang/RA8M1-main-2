#include "randombytes.h"
#include <string.h>

#define ROTATE(x, b) (((x) << (b)) | ((x) >> (32 - (b))))
#define MUSH(i, b) x = t[i] += (((x ^ seed[i]) + sum) ^ ROTATE(x, b));

static uint32_t seed[32] = {9, 7, 0, 1, 7, 6, 2, 9, 5, 3, 5, 9, 9, 7, 4, 3,
                            1, 5, 0, 6, 6, 8, 5, 4, 7, 3, 8, 3, 2, 7, 9, 5};
static uint32_t in[12];
static uint8_t out_buf[sizeof(uint32_t) * 16];
static int32_t outleft = 0;

void surf(uint32_t out[8])
{
  uint32_t t[12];
  uint32_t x;
  uint32_t sum = 0;
  int32_t r;
  int32_t i;
  int32_t loop;

  for (i = 0; i < 12; ++i) {
    t[i] = in[i] ^ seed[12 + i];
  }
  for (i = 0; i < 8; ++i) {
    out[i] = seed[24 + i];
  }
  x = t[11];
  for (loop = 0; loop < 2; ++loop) {
    for (r = 0; r < 16; ++r) {
      sum += 0x9e3779b9;
      MUSH(0, 5)
      MUSH(1, 7)
      MUSH(2, 9)
      MUSH(3, 13)
      MUSH(4, 5)
      MUSH(5, 7)
      MUSH(6, 9)
      MUSH(7, 13)
      MUSH(8, 5)
      MUSH(9, 7)
      MUSH(10, 9)
      MUSH(11, 13)
    }
    for (i = 0; i < 8; ++i) {
      out[i] ^= t[i + 4];
    }
  }
}

void randombytes_regen(void)
{
  uint32_t out[8];
  if (!++in[0]) {
    if (!++in[1]) {
      if (!++in[2]) {
        ++in[3];
      }
    }
  }
  surf(out);
  memcpy(out_buf, out, sizeof(out));
  if (!++in[0]) {
    if (!++in[1]) {
      if (!++in[2]) {
        ++in[3];
      }
    }
  }
  surf(out);
  memcpy(out_buf + sizeof(out), out, sizeof(out));
  outleft = sizeof(out_buf);
}

int randombytes(uint8_t* buf, size_t xlen)
{
  while (xlen > 0) {
    if (!outleft) {
      randombytes_regen();
    }
    *buf = out_buf[--outleft];
    ++buf;
    --xlen;
  }
  return 0;
}

int random_4bits_in_a_byte(uint8_t* buf, size_t xlen)
{
  while (xlen > 0) {
    if (!outleft) {
      randombytes_regen();
    }
    *buf = out_buf[--outleft] & 0x0f;
    ++buf;
    --xlen;
  }
  return 0;
}