#include <stdint.h>
#include <stdio.h>
#include "xoshiro128.h"

// Left rotation helper macro
static inline uint32_t rotl(const uint32_t x, int k) {
	return (x << k) | (x >> (32 - k));
}

uint32_t splitmix32(uint32_t *state) {
	uint32_t z = (*state += 0x9e3779b9);
	z = (z ^ (z >> 16)) * 0x21f0aaad;
	z = (z ^ (z >> 15)) * 0x735a2d97;
	return z ^ (z >> 15);
}
/**
* Initializes the xoshiro128** instance state.
* Note: The state must NOT be all zeros. If an all-zero seed is passed, 
* a fallback value should fill the remaining states.
*/
void xoshiro128_seed(Xoshiro128Ctx* ctx, uint32_t seed) {
	// Basic splitmix32-like or simple expansion to fill the 4 state slots from one seed

	uint32_t sm_state = seed;
	ctx->s[0] = splitmix32(&sm_state);
	ctx->s[1] = splitmix32(&sm_state);
	ctx->s[2] = splitmix32(&sm_state);
	ctx->s[3] = splitmix32(&sm_state);	
	//printf("%d\n", ctx->s[0]);
	//printf("%d\n", ctx->s[1]);
	//printf("%d\n", ctx->s[2]);
	//printf("%d\n", ctx->s[3]);
}

/**
* Generates the next 32-bit unsigned integer for this specific instance.
*/
uint32_t xoshiro128_next(Xoshiro128Ctx* ctx) {
	const uint32_t result = rotl(ctx->s[1] * 5, 7) * 9;

	const uint32_t t = ctx->s[1] << 9;

	ctx->s[2] ^= ctx->s[0];
	ctx->s[3] ^= ctx->s[1];
	ctx->s[1] ^= ctx->s[2];
	ctx->s[0] ^= ctx->s[3];

	ctx->s[2] ^= t;

	ctx->s[3] = rotl(ctx->s[3], 11);

	return result;
}

/**
* https://www.jameswhanlon.com/seeding-parallel-xoroshiro128-generators.html
* This is the jump function for the generator. It is equivalent
* to 2^64 calls to next(); it can be used to create 2^64 non-overlapping
* parallel streams from the same initial seed state.
*/
void xoshiro128_jump(Xoshiro128Ctx* ctx) {
	static const uint32_t JUMP[] = { 0x8764000b, 0xf542d2d3, 0x6fa035c3, 0x77f2db5b };

	uint32_t s0 = 0;
	uint32_t s1 = 0;
	uint32_t s2 = 0;
	uint32_t s3 = 0;

	for (int i = 0; i < sizeof JUMP / sizeof *JUMP; i++) {
		for (int b = 0; b < 32; b++) {
			if (JUMP[i] & UINT32_C(1) << b) {
				s0 ^= ctx->s[0];
				s1 ^= ctx->s[1];
				s2 ^= ctx->s[2];
				s3 ^= ctx->s[3];
			}
			xoshiro128_next(ctx);
		}
	}

	ctx->s[0] = s0;
	ctx->s[1] = s1;
	ctx->s[2] = s2;
	ctx->s[3] = s3;
}
