#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "sha1.h"

typedef struct {
    uint32_t state[5];
    uint64_t count;
    unsigned char buffer[64];
} sha1_ctx;

static uint32_t sha1_rol(uint32_t x, int n) { return (x << n) | (x >> (32 - n)); }

static void sha1_transform(sha1_ctx* ctx, const unsigned char* block) {
    uint32_t w[80];
    for (int i = 0; i < 16; i++)
        w[i] = ((uint32_t)block[i * 4] << 24) | ((uint32_t)block[i * 4 + 1] << 16) |
               ((uint32_t)block[i * 4 + 2] << 8) | block[i * 4 + 3];
    for (int i = 16; i < 80; i++)
        w[i] = sha1_rol(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);

    uint32_t a = ctx->state[0], b = ctx->state[1], c = ctx->state[2],
             d = ctx->state[3], e = ctx->state[4];
    for (int i = 0; i < 80; i++) {
        uint32_t f, k;
        if (i < 20)       { f = (b & c) | (~b & d);        k = 0x5A827999; }
        else if (i < 40)  { f = b ^ c ^ d;                 k = 0x6ED9EBA1; }
        else if (i < 60)  { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDC; }
        else              { f = b ^ c ^ d;                 k = 0xCA62C1D6; }
        uint32_t tmp = sha1_rol(a, 5) + f + e + k + w[i];
        e = d; d = c; c = sha1_rol(b, 30); b = a; a = tmp;
    }
    ctx->state[0] += a; ctx->state[1] += b; ctx->state[2] += c;
    ctx->state[3] += d; ctx->state[4] += e;
}


static void sha1_init(sha1_ctx* ctx) {
    ctx->state[0] = 0x67452301; ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE; ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
    ctx->count = 0;
}

static void sha1_update(sha1_ctx* ctx, const unsigned char* data, size_t len) {
    size_t i = (size_t)(ctx->count % 64);
    ctx->count += len;
    while (len--) {
        ctx->buffer[i++] = *data++;
        if (i == 64) { sha1_transform(ctx, ctx->buffer); i = 0; }
    }
}

static void sha1_final(sha1_ctx* ctx, unsigned char out[20]) {
    uint64_t bits = ctx->count * 8;
    unsigned char pad[72] = {0x80};   /* 0x80 followed by zeros */
    size_t i = (size_t)(ctx->count % 64);
    size_t zeros = i < 56 ? 56 - i : 120 - i;
    sha1_update(ctx, pad, zeros);
    unsigned char len_buf[8];
    for (int j = 0; j < 8; j++) len_buf[j] = (unsigned char)(bits >> (56 - 8 * j));
    sha1_update(ctx, len_buf, 8);
    for (int j = 0; j < 5; j++) {
        out[j * 4]     = (unsigned char)(ctx->state[j] >> 24);
        out[j * 4 + 1] = (unsigned char)(ctx->state[j] >> 16);
        out[j * 4 + 2] = (unsigned char)(ctx->state[j] >> 8);
        out[j * 4 + 3] = (unsigned char)ctx->state[j];
    }
}

void sha1_hex(const char* s, char out[41]) {
    sha1_ctx ctx;
    unsigned char digest[20];
    sha1_init(&ctx);
    sha1_update(&ctx, (const unsigned char*)s, strlen(s));
    sha1_final(&ctx, digest);
    for (int i = 0; i < 20; i++) sprintf(out + i * 2, "%02x", digest[i]);
    out[40] = '\0';
}

