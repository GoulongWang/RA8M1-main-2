#ifndef RANDOMBYTES_H
#define RANDOMBYTES_H
#include <stdint.h>
#include <unistd.h>

void surf(uint32_t out[8]);
void randombytes_regen(void);
int randombytes(uint8_t *buf, size_t n);
int random_4bits_in_a_byte(uint8_t* buf, size_t xlen);

#endif