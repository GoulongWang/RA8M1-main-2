/*
 * AES implementation based on code from BearSSL (https://bearssl.org/)
 * by Thomas Pornin.
 *
 *
 * Copyright (c) 2016 Thomas Pornin <pornin@bolet.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdint.h>
#include <string.h>

#include "aes.h"
#include "aes128_4r_ffs.h"

#ifdef PROFILE_HASHING
#include "hal.h"
extern unsigned long long hash_cycles;
#endif

#define aes128_keyexp_asm aes128_keyexp_publicinputs_asm

#define aes128_encrypt_asm 
extern void aes128_keyexp_publicinputs_asm(const uint8_t *key,uint8_t *rk);
//extern void aes128_keyexp_asm(const uint8_t *key, uint8_t *rk);
extern void aes192_keyexp_asm(const uint8_t *key, uint8_t *rk);
extern void aes256_keyexp_asm(const uint8_t *key, uint8_t *rk);
//extern void aes128_encrypt_asm(const uint8_t *rk, const uint8_t *in, uint8_t *out);
extern void aes192_encrypt_asm(const uint8_t *rk, const uint8_t *in, uint8_t *out);
extern void aes256_encrypt_asm(const uint8_t *rk, const uint8_t *in, uint8_t *out);

static inline uint32_t br_swap32(uint32_t x) {
    x = ((x & (uint32_t)0x00FF00FF) << 8)
        | ((x >> 8) & (uint32_t)0x00FF00FF);
    return (x << 16) | (x >> 16);
}


static inline void inc1_be(uint32_t *x) {
    uint32_t t = br_swap32(*x) + 1;
    *x = br_swap32(t);
}


static void aes_ecb(unsigned char *out, const unsigned char *in, size_t nblocks, const uint64_t *rkeys, void (*aes_encrypt_asm)(const uint8_t *, const uint8_t *, uint8_t *)) {
    unsigned int i;
    for (i = 0; i < nblocks; ++i) {
        aes_encrypt_asm((uint8_t *)rkeys, in, out);
        in += AES_BLOCKBYTES;
        out += AES_BLOCKBYTES;
    }
}

static void aes_ctr(unsigned char *out, size_t outlen, const unsigned char *iv, const uint64_t *rkeys, void (*aes_encrypt_asm)(const uint8_t *, const uint8_t *, uint8_t *)) {
    uint32_t ivw[4] = {0};
    uint8_t buf[AES_BLOCKBYTES];
    size_t i;

    memcpy(ivw, iv, AESCTR_NONCEBYTES);

    while (outlen > AES_BLOCKBYTES) {
        aes_encrypt_asm((uint8_t *)rkeys, (uint8_t *)ivw, out);
        inc1_be(ivw + 3);
        out += AES_BLOCKBYTES;
        outlen -= AES_BLOCKBYTES;
    }
    if (outlen > 0) {
        aes_encrypt_asm((unsigned char *)rkeys, (unsigned char *)ivw, buf);
        for (i = 0; i < outlen; i++) {
            out[i] = buf[i];
        }
    }
}
 
static void aes128_keyexp(aes128ctx *r, const unsigned char *key) {
#ifdef PROFILE_HASHING
    uint64_t t0 = hal_get_time();
#endif

    memcpy((uint8_t *)r->sk_exp, key, AES128_KEYBYTES);
    aes128_keyexp_asm(key, ((uint8_t *)r->sk_exp) + AES128_KEYBYTES);

#ifdef PROFILE_HASHING
    uint64_t t1 = hal_get_time();
    hash_cycles += (t1-t0);
#endif
}

void aes128_ecb_keyexp(aes128ctx *r, const unsigned char *key) {
    aes128_keyexp(r, key);
}

void aes128_ctr_keyexp(aes128ctx *r, const unsigned char *key) {
    aes128_keyexp(r, key);
}

static void aes192_keyexp(aes192ctx *r, const unsigned char *key) {
#ifdef PROFILE_HASHING
    uint64_t t0 = hal_get_time();
#endif

    memcpy((uint8_t *)r->sk_exp, key, AES192_KEYBYTES);
    aes192_keyexp_asm(key, ((uint8_t *)r->sk_exp) + AES192_KEYBYTES);

#ifdef PROFILE_HASHING
    uint64_t t1 = hal_get_time();
    hash_cycles += (t1-t0);
#endif
}


void aes192_ecb_keyexp(aes192ctx *r, const unsigned char *key) {
    aes192_keyexp(r, key);
}

void aes192_ctr_keyexp(aes192ctx *r, const unsigned char *key) {
    aes192_keyexp(r, key);
}


static void aes256_keyexp(aes256ctx *r, const unsigned char *key) {
#ifdef PROFILE_HASHING
    uint64_t t0 = hal_get_time();
#endif

    memcpy((uint8_t *)r->sk_exp, key, AES256_KEYBYTES);
    aes256_keyexp_asm(key, ((uint8_t *)r->sk_exp) + AES256_KEYBYTES);

#ifdef PROFILE_HASHING
    uint64_t t1 = hal_get_time();
    hash_cycles += (t1-t0);
#endif
}

void aes256_ecb_keyexp(aes256ctx *r, const unsigned char *key) {
    aes256_keyexp(r, key);
}

void aes256_ctr_keyexp(aes256ctx *r, const unsigned char *key) {
    aes256_keyexp(r, key);
}

/* 
void aes128_ecb(unsigned char *out, const unsigned char *in, size_t nblocks, const aes128ctx *ctx) {
#ifdef PROFILE_HASHING
    uint64_t t0 = hal_get_time();
#endif

    aes_ecb(out, in, nblocks, ctx->sk_exp, aes128_encrypt_asm);

#ifdef PROFILE_HASHING
    uint64_t t1 = hal_get_time();
    hash_cycles += (t1-t0);
#endif
} */

void aes128_ecb(unsigned char *out, const unsigned char *in, size_t nblocks, const aes128ctx *ctx){
    #ifdef PROFILE_HASHING
    uint64_t t0 = hal_get_time();   
    #endif
    uint8_t buf0[AES_BLOCKBYTES], buf1[AES_BLOCKBYTES];

    while(nblocks > 0){
        if(nblocks >= 2){
            aes128_encrypt_ffs(out, out+AES_BLOCKBYTES, in, in+AES_BLOCKBYTES, (uint32_t*) ctx->sk_exp);
            out += AES_BLOCKBYTES*2;
            in += AES_BLOCKBYTES*2;
            nblocks -= 2;
        } else {
            aes128_encrypt_ffs(out, buf0, in, buf1, (uint32_t*) ctx->sk_exp);
            nblocks--;
        }
    }

    #ifdef PROFILE_HASHING
    uint64_t t1 = hal_get_time();
    hash_cycles += (t1-t0);
    #endif
}

/* void aes128_ctr(unsigned char *out, size_t outlen, const unsigned char *iv, uint32_t ctr, const aes128ctx *ctx) {
#ifdef PROFILE_HASHING
    uint64_t t0 = hal_get_time();
#endif

    aes_ctr(out, outlen, iv, ctx->sk_exp, aes128_encrypt_asm);

#ifdef PROFILE_HASHING
    uint64_t t1 = hal_get_time();
    hash_cycles += (t1-t0);
#endif
} */

// copied from https://github.com/mupq/pqm4/blob/a24bb4b662016968c19f5e6a0719c9ad530f0286/common/aes.c#L89C1-L131C2

static inline void inc2_be(uint32_t *x) {
    uint32_t t = br_swap32(*x) + 2;
    *x = br_swap32(t);
}

void aes128_ctr(unsigned char *out, size_t outlen, const unsigned char *iv, uint32_t ctr, const aes128ctx *ctx){
    #ifdef PROFILE_HASHING
    uint64_t t0 = hal_get_time();
    #endif
    uint32_t ivw1[4] = {0};
    uint32_t ivw2[4] = {0};
    uint8_t buf1[AES_BLOCKBYTES];
    uint8_t buf2[AES_BLOCKBYTES];
    size_t i;

    memcpy(ivw1, iv, AESCTR_NONCEBYTES);
    memcpy(ivw2, iv, AESCTR_NONCEBYTES);
    inc1_be(ivw2 + 3);

    while (outlen > 2*AES_BLOCKBYTES) {
        aes128_encrypt_ffs(out, out+16, (uint8_t *)ivw1, (uint8_t *)ivw2, (uint32_t*)ctx->sk_exp);
        inc2_be(ivw1 + 3);
        inc2_be(ivw2 + 3);
        out += AES_BLOCKBYTES*2;
        outlen -= AES_BLOCKBYTES*2;
    }
    if (outlen >= AES_BLOCKBYTES) {
        aes128_encrypt_ffs(out, buf2, (uint8_t *)ivw1, (uint8_t *)ivw2, (uint32_t*)ctx->sk_exp);
        out += AES_BLOCKBYTES;
        outlen -= AES_BLOCKBYTES;
        for (i = 0; i < outlen; i++) {
            out[i] = buf2[i];
        }
        } else if (outlen > 0) {
            aes128_encrypt_ffs(buf1, buf2, (uint8_t *)ivw1, (uint8_t *)ivw2, (uint32_t*)ctx->sk_exp);
            for (i = 0; i < outlen; i++) {
                out[i] = buf1[i];
            }
        }

    #ifdef PROFILE_HASHING
    uint64_t t1 = hal_get_time();
    hash_cycles += (t1-t0);
    #endif
}

void aes192_ecb(unsigned char *out, const unsigned char *in, size_t nblocks, const aes192ctx *ctx) {
#ifdef PROFILE_HASHING
    uint64_t t0 = hal_get_time();
#endif

    aes_ecb(out, in, nblocks, ctx->sk_exp, aes192_encrypt_asm);

#ifdef PROFILE_HASHING
    uint64_t t1 = hal_get_time();
    hash_cycles += (t1-t0);
#endif
}

void aes192_ctr(unsigned char *out, size_t outlen, const unsigned char *iv, const aes192ctx *ctx) {
#ifdef PROFILE_HASHING
    uint64_t t0 = hal_get_time();
#endif

    aes_ctr(out, outlen, iv, ctx->sk_exp, aes192_encrypt_asm);

#ifdef PROFILE_HASHING
    uint64_t t1 = hal_get_time();
    hash_cycles += (t1-t0);
#endif
}

void aes256_ecb(unsigned char *out, const unsigned char *in, size_t nblocks, const aes256ctx *ctx) {
#ifdef PROFILE_HASHING
    uint64_t t0 = hal_get_time();
#endif

    aes_ecb(out, in, nblocks, ctx->sk_exp, aes256_encrypt_asm);

#ifdef PROFILE_HASHING
    uint64_t t1 = hal_get_time();
    hash_cycles += (t1-t0);
#endif
}

void aes256_ctr(unsigned char *out, size_t outlen, const unsigned char *iv, const aes256ctx *ctx) {
#ifdef PROFILE_HASHING
    uint64_t t0 = hal_get_time();
#endif

    aes_ctr(out, outlen, iv, ctx->sk_exp, aes256_encrypt_asm);

#ifdef PROFILE_HASHING
    uint64_t t1 = hal_get_time();
    hash_cycles += (t1-t0);
#endif
}

void aes128_ctx_release(aes128ctx *r) {
    // no-op for mupq's basic AES operation
    // this is required for compatibility with code from PQClean
    // see https://github.com/PQClean/PQClean/pull/198
    (void) r;
}

void aes192_ctx_release(aes192ctx *r) {
    // no-op for mupq's basic AES operation
    // this is required for compatibility with code from PQClean
    // see https://github.com/PQClean/PQClean/pull/198
    (void) r;
}

void aes256_ctx_release(aes256ctx *r) {
    // no-op for mupq's basic AES operation
    // this is required for compatibility with code from PQClean
    // see https://github.com/PQClean/PQClean/pull/198
    (void) r;
}

