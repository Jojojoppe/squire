#ifndef __H_SQUIRE_CRYPT
#define __H_SQUIRE_CRYPT 1

#include <stdint.h>
#include <stddef.h>

extern void squire_chacha_block(uint32_t out[16], uint32_t const in[16]);
extern void squire_chacha(const char key[32], uint64_t pos, uint64_t nonce, uint8_t * buf, size_t length);

#endif