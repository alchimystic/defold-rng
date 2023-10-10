#pragma once

#include <math.h>

class Pcg32
{
	private:
	pcg32_random_t state;

	public:
	Pcg32(): state(pcg_initializer()) {};

	~Pcg32() { Release(); }

	void set_seed(uint64_t stat, uint64_t seq) {
		pcg32_srandom_r(&state, stat, seq);
	}

	uint32_t number() {
		return pcg32_random_r(&state);
	}

	uint32_t range(uint32_t min, uint32_t max) {
		max++;
		return pcg32_boundedrand_r(&state, (max - min)) + min;
	}

	double double_range(uint32_t min, uint32_t max) {
		uint32_t num = pcg32_random_r(&state);
		return (double)(num) / ((double)UINT32_MAX) * (max - min) + min;
	}
	
	double double_num() {
		return ldexp(pcg32_random_r(&state), -32);
	}

	int roll() {
		return pcg32_boundedrand_r(&state, 6);
	}

	int toss() {
		return pcg32_boundedrand_r(&state, 2);
	}
		
	void Release() {
		//release code goes here
	}    
};
