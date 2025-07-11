// SPDX-License-Identifier: Apache-2.0 or CC0-1.0
#ifndef AES_H
#define AES_H

#include <stdint.h>
#include <stdlib.h>

#define AES128_KEYBYTES 16
#define AES192_KEYBYTES 24
#define AES256_KEYBYTES 32
#define AESCTR_NONCEBYTES 12
#define AES_BLOCKBYTES 16

typedef struct {
    uint64_t sk_exp[88];
} aes128ctx;

typedef struct {
    uint64_t sk_exp[104];
} aes192ctx;

typedef struct {
    uint64_t sk_exp[120];
} aes256ctx;



/** Initializes the context **/
void aes128_ecb_keyexp(aes128ctx *r, const unsigned char *key);

void aes128_ctr_keyexp(aes128ctx *r, const unsigned char *key);

void aes128_ecb(unsigned char *out, const unsigned char *in, size_t nblocks, const aes128ctx *ctx);

void aes128_ctr(unsigned char *out, size_t outlen, const unsigned char *iv, uint32_t ctr, const aes128ctx *ctx);

void aes128_ctx_release(aes128ctx *r);

/** Initializes the context **/
void aes192_ecb_keyexp(aes192ctx *r, const unsigned char *key);

void aes192_ctr_keyexp(aes192ctx *r, const unsigned char *key);

void aes192_ecb(unsigned char *out, const unsigned char *in, size_t nblocks, const aes192ctx *ctx);

void aes192_ctr(unsigned char *out, size_t outlen, const unsigned char *iv, const aes192ctx *ctx);

void aes192_ctx_release(aes192ctx *r);


/** Initializes the context **/
void aes256_ecb_keyexp(aes256ctx *r, const unsigned char *key);

void aes256_ctr_keyexp(aes256ctx *r, const unsigned char *key);

void aes256_ecb(unsigned char *out, const unsigned char *in, size_t nblocks, const aes256ctx *ctx);

void aes256_ctr(unsigned char *out, size_t outlen, const unsigned char *iv, const aes256ctx *ctx);

void aes256_ctx_release(aes256ctx *r);


#endif
