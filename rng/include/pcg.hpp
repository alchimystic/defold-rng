#pragma once

#include "pcg_basic.h"

#include <math.h>
#include <stdio.h>

extern "C" {
	#include "lua/lua.h"
	#include "lua/lauxlib.h"
	#include "lua/lualib.h"
}


class Pcg32
{
	private:
	pcg32_random_t state;

	public:
	Pcg32(): state(pcg_initializer()) {};

	~Pcg32() { Release(); }

	void set_seed(uint64_t stat, uint64_t inc) {
		pcg32_srandom_r(&state, stat, inc);
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

class LuaPcg32 {
	static const char className[];
	static const luaL_reg methods[];

	public:
	static void Register(lua_State* L);
	static int create(lua_State *L);
	
	private:
	static Pcg32* checkInstance(lua_State *L, int narg);
	static int range_error(lua_State *L);
	static int fixed_value(lua_State *L, double value);
	static uint64_t* new_seed();
	static int seed(lua_State *l);
	static int number(lua_State *L);
	static int double_num(lua_State *L);
	static int range(lua_State *L);
	static int double_range(lua_State *L);
	static int roll(lua_State *L);
	static int toss(lua_State *L);
	static int gc_pcg(lua_State *L);	
};


