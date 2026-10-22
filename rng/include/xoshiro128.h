
// Instance state encapsulation (No static variables)
typedef struct {
	uint32_t s[4]; 
} Xoshiro128Ctx;

void xoshiro128_seed(Xoshiro128Ctx* ctx, uint32_t seed);

uint32_t xoshiro128_next(Xoshiro128Ctx* ctx);

void xoshiro128_jump(Xoshiro128Ctx* ctx);
