
#define LIB_NAME "rng"
#define MODULE_NAME "rng"

#include "entropy.h"
#include "pcg_basic.h"
#include "pcg.hpp"

extern "C" {
    #include "lua/lua.h"
    #include "lua/lauxlib.h"
    #include "lua/lualib.h"
}

//#include <math.h>
//#include <dmsdk/dlib/log.h>
//#include <dmsdk/sdk.h>

#define lua_boxpointer(L,u) (*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))
#define lua_unboxpointer(L,i)   (*(void **)(lua_touserdata(L, i)))
#define method(class, name) {#name, class::name}

Pcg32* LuaPcg32::checkInstance(lua_State *L, int narg) {
    luaL_checktype(L, narg, LUA_TUSERDATA);
    void *ud = luaL_checkudata(L, narg, className);
    if(!ud) luaL_typerror(L, narg, className);
    return *(Pcg32**)ud;  // unbox pointer
}

int LuaPcg32::range_error(lua_State *L) {
    lua_pushliteral(L, "min cannot be bigger than max");
    lua_error(L);
    return 1;        
}

int LuaPcg32::fixed_value(lua_State *L, double value) {
    lua_pushnumber(L, value);
    return 1;
}

uint64_t* LuaPcg32::new_seed() {
    uint64_t result[2] = {0, 0};
    entropy_getbytes((void *)result, sizeof(result));
    return result;
}

int LuaPcg32::seed(lua_State *l) {
    Pcg32 *o = checkInstance(l, 1);
    long state = luaL_optinteger(l, 2, 0);
    long inc = 0;

    if (state == 0) {
        uint64_t* seeds = new_seed();          
        state = seeds[0];
        inc = seeds[1];        
    } else {
        inc = luaL_optinteger(l, 3, 0);
    }

    o->set_seed(state, inc);
    return 0;
}

int LuaPcg32::number(lua_State *L) {
    Pcg32 *o = checkInstance(L, 1);
    uint32_t result = o->number();
    lua_pushnumber(L, result);
    return 1;
}

int LuaPcg32::double_num(lua_State *L) {
    Pcg32 *o = checkInstance(L, 1);
    double result = o->double_num();
    lua_pushnumber(L, result);
    return 1;
}

int LuaPcg32::range(lua_State *L) {
    Pcg32 *o = checkInstance(L, 1);
    uint32_t min = luaL_checknumber(L, 2);
    uint32_t max = luaL_checknumber(L, 3);

    if (min == max)
    {
        return fixed_value(L, min);
    }

    if (min > max)
    {
        return range_error(L);
    }

    uint32_t result = o->range(min, max);
    lua_pushnumber(L, result);
    return 1;
}

int LuaPcg32::double_range(lua_State *L) {
    Pcg32 *o = checkInstance(L, 1);
    double min = luaL_checknumber(L, 2);
    double max = luaL_checknumber(L, 3);

    if (min == max)
    {
        return fixed_value(L, min);
    }

    if (min > max)
    {
        return range_error(L);
    }

    double result = o->double_range(min, max);
    lua_pushnumber(L, result);
    return 1;
}

int LuaPcg32::roll(lua_State *L) {
    Pcg32 *o = checkInstance(L, 1);
    int result = o->roll();
    lua_pushnumber(L, result);
    return 1;        
}

int LuaPcg32::toss(lua_State *L) {
    Pcg32 *o = checkInstance(L, 1);
    int result = o->toss();
    lua_pushnumber(L, result);
    return 1;        
}

int LuaPcg32::gc_pcg(lua_State *L) {
    Pcg32 *o = (Pcg32*)lua_unboxpointer(L, 1);
    delete o;
    return 0;
}

void LuaPcg32::Register(lua_State* L) {
    lua_newtable(L);                 
    int methodtable = lua_gettop(L);
    luaL_newmetatable(L, className); 
    int metatable   = lua_gettop(L);

    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, methodtable);
    lua_settable(L, metatable);  // hide metatable from Lua getmetatable()

    lua_pushliteral(L, "__index");
    lua_pushvalue(L, methodtable);
    lua_settable(L, metatable);

    lua_pushliteral(L, "__gc");
    lua_pushcfunction(L, gc_pcg);
    lua_settable(L, metatable);

    lua_pop(L, 1);  // drop metatable

    luaL_openlib(L, 0, methods, 0);  // fill methodtable
    lua_pop(L, 1);  // drop methodtable

    lua_register(L, className, create);
}

int LuaPcg32::create(lua_State *L) {

    int arg_count = lua_gettop(L);
    Pcg32 *o = new Pcg32();

    if (arg_count > 0) {
        long stat = luaL_checknumber(L, 1);
        long inc = luaL_checknumber(L, 2);
        o->set_seed(stat, inc);    
    }

    lua_boxpointer(L, o);
    luaL_getmetatable(L, className);
    lua_setmetatable(L, -2);
    return 1;
}

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
