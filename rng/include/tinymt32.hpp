#pragma once

#include "tinymt32.h"
#include "entropy.h"

#include <math.h>
#include <stdio.h>

class TinyMT32
{
	private:
	tinymt32_t state;

	tinymt32_t initial_state() {
		tinymt32_t result;
		result.mat1 = 0x8f7011ee;
		result.mat2 = 0xfc78ff1f;
		result.tmat = 0x3793fdff;
		return result;
	}
	
	public:
	TinyMT32(): state(initial_state()) {};

	~TinyMT32() { Release(); }

	void set_seed(uint32_t seed) {
		tinymt32_init(&state, seed);
	}

	void set_seed_by_array(uint32_t* seed) {
		tinymt32_init_by_array(&state, seed, sizeof(seed));
	}
	
	uint32_t number() {
		return tinymt32_generate_uint32(&state);
	}

	uint32_t range(uint32_t min, uint32_t max) {
		double r = tinymt32_generate_32double(&state);
		uint32_t d = max - min + 1;
		return d * r + min;
	}

	double double_range(uint32_t min, uint32_t max) {
		double r = tinymt32_generate_32double(&state);
		uint32_t d = max - min;
		return r * d + min;
	}

	double double_num() {
		return tinymt32_generate_32double(&state);
	}

	void Release() {
		//release code goes here
	}

	static uint32_t* random_seed(int length) {
		uint64_t* array = new uint64_t[length];
		entropy_getbytes((void *)array, length);
		uint32_t* result = new uint32_t[length];
		for (int i = 0; i < length; i++) {
			result[i] = (uint32_t)array[i];
		}
		
		return result;
	}
};
