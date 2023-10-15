
#define LIB_NAME "rng"
#define MODULE_NAME "rng"

#include "pcg.hpp"

extern "C" {
	#include "lua/lua.h"
	#include "lua/lauxlib.h"
	#include "lua/lualib.h"
}

#include <dmsdk/dlib/log.h>
#include <dmsdk/sdk.h>

#define lua_boxpointer(L,u) (*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))
#define lua_unboxpointer(L,i)   (*(void **)(lua_touserdata(L, i)))
#define method(class, name) {#name, class::name}

class Rng
{
	public:
	Rng() {};

	static int pcg32(lua_State *l) {
		LuaPcg32::create(l);
		return 1;
	}
};

static const struct luaL_reg rnglib [] =
{
	{ "pcg32", Rng::pcg32 },
	{ NULL, NULL }
};

const char LuaPcg32::className[] = "Pcg32";

const luaL_reg LuaPcg32::methods[] = {
	method(LuaPcg32, seed),
	method(LuaPcg32, number),
	{ "double", LuaPcg32::double_num },
	method(LuaPcg32, range),
	method(LuaPcg32, double_range),
	method(LuaPcg32, roll),
	method(LuaPcg32, toss),
	{0,0}
};


static void RngLuaInit(lua_State *l)
{
	int top = lua_gettop(l);

	luaL_newmetatable(l, "rng");
	lua_pushvalue(l, -1);
	lua_setfield(l, -2, "__index");

	luaL_register(l, "rng", rnglib);

	// Register lua names
	lua_pop(l, 1);
	lua_pop(l, 1);
	assert(top == lua_gettop(l));
}

dmExtension::Result app_init_rng(dmExtension::AppParams *params)
{
	return dmExtension::RESULT_OK;
}

dmExtension::Result init_rng(dmExtension::Params *params)
{
	RngLuaInit(params->m_L); //registers rng
	LuaPcg32::Register(params->m_L); //registers pcg32

	dmLogInfo("Registered %s Extension\n", MODULE_NAME);
	return dmExtension::RESULT_OK;
}

dmExtension::Result app_final_rng(dmExtension::AppParams *params)
{
	return dmExtension::RESULT_OK;
}

dmExtension::Result final_rng(dmExtension::Params *params)
{
	return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(rng, LIB_NAME, app_init_rng, app_final_rng, init_rng, 0, 0, final_rng)