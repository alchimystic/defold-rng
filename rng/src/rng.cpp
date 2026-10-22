
#define LIB_NAME "rng"
#define MODULE_NAME "rng"

#include "entropy.h"
#include "pcg_basic.h"
#include "pcg.h"
#include "xoshiro128.h"
#include "tinymt32.hpp"
#include <dmsdk/dlib/log.h>
#include <dmsdk/sdk.h>
#include <math.h>
#include <time.h>
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#define lua_boxpointer(L,u) (*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))
#define lua_unboxpointer(L,i)   (*(void **)(lua_touserdata(L, i)))
#define method(class, name) {#name, class::name}


static int range_error(lua_State *L) {
    lua_pushliteral(L, "min cannot be bigger than max");
    lua_error(L);
    return 1;        
}

static int fixed_value(lua_State *L, double value) {
    lua_pushnumber(L, value);
    return 1;
}

static void RegisterRng(
    lua_State* L, 
    char const* className,  
    const luaL_reg* methods, 
    lua_CFunction create, 
    lua_CFunction destroy) 
{
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

    lua_pop(L, 1);  // drop metatable

    luaL_openlib(L, 0, methods, 0);  // fill methodtable
    lua_pop(L, 1);  // drop methodtable

    lua_register(L, className, create);
}


class LuaPcg32 {
    static const char className[];
    static const luaL_reg methods[];

    static Pcg32 *checkInstance(lua_State *L, int narg) {
        luaL_checktype(L, narg, LUA_TUSERDATA);
        void *ud = luaL_checkudata(L, narg, className);
        if(!ud) luaL_typerror(L, narg, className);
        return *(Pcg32**)ud;  // unbox pointer
    }

    static uint64_t * new_seed() {
        uint64_t result[2] = {0, 0};
        entropy_getbytes((void *)result, sizeof(result));
        return result;
    }

    static int seed(lua_State *l) {
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
    
    static int number(lua_State *L) {
        Pcg32 *o = checkInstance(L, 1);
        uint32_t result = o->number();
        lua_pushnumber(L, result);
        return 1;
    }

    static int double_num(lua_State *L) {
        Pcg32 *o = checkInstance(L, 1);
        double result = o->double_num();
        lua_pushnumber(L, result);
        return 1;
    }

    static int range(lua_State *L) {
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

    static int double_range(lua_State *L) {
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

        double result = o->double_range(min, max);
        lua_pushnumber(L, result);
        return 1;
    }

    static int roll(lua_State *L) {
        Pcg32 *o = checkInstance(L, 1);
        int result = o->roll();
        lua_pushnumber(L, result);
        return 1;        
    }

    static int toss(lua_State *L) {
        Pcg32 *o = checkInstance(L, 1);
        int result = o->toss();
        lua_pushnumber(L, result);
        return 1;        
    }

    static int gc_pcg(lua_State *L) {
        Pcg32 *o = (Pcg32*)lua_unboxpointer(L, 1);
        delete o;
        return 0;
    }
        
    public:
    static void Register(lua_State *l) {
        //LuaPcg32::Register(l);
        RegisterRng(l, className, methods, create, gc_pcg);
    }
    static int create(lua_State *L) {

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
};


class LuaTinyMT32 {
    static const char className[];
    static const luaL_reg methods[];

    static TinyMT32* checkInstance(lua_State *L, int narg) {
        luaL_checktype(L, narg, LUA_TUSERDATA);
        void *ud = luaL_checkudata(L, narg, className);
        if(!ud) luaL_typerror(L, narg, className);
        return *(TinyMT32**)ud;  // unbox pointer
    }

    static uint32_t* new_seed() {        
        return TinyMT32::random_seed(4);
    }

    static int seed(lua_State *l) {
        TinyMT32 *o = checkInstance(l, 1);
        set_seed(o, luaL_optinteger(l, 2, 0));
        return 0;
    }

    static int number(lua_State *L) {
        TinyMT32 *o = checkInstance(L, 1);
        uint32_t result = o->number();
        lua_pushnumber(L, result);
        return 1;
    }

    static int double_num(lua_State *L) {
        TinyMT32 *o = checkInstance(L, 1);
        double result = o->double_num();
        lua_pushnumber(L, result);
        return 1;
    }

    static int range(lua_State *L) {
        TinyMT32 *o = checkInstance(L, 1);
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

    static int double_range(lua_State *L) {
        TinyMT32 *o = checkInstance(L, 1);
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

    static int roll(lua_State *L) {
        TinyMT32 *o = checkInstance(L, 1);
        int result = o->range(1, 6);
        lua_pushnumber(L, result);
        return 1;        
    }

    static int toss(lua_State *L) {
        TinyMT32 *o = checkInstance(L, 1);
        int result = o->range(0, 1);
        lua_pushnumber(L, result);
        return 1;        
    }

    static int gc_tmt(lua_State *L) {
        TinyMT32 *o = (TinyMT32*)lua_unboxpointer(L, 1);
        delete o;
        return 0;
    }

    static void set_seed(TinyMT32 *obj, uint32_t seed) {
        if (seed == 0) {
            obj->set_seed_by_array(new_seed());
        } else {
            obj->set_seed(seed);
        }
    }
    
    public:
    static void Register(lua_State *l) {
        RegisterRng(l, className, methods, create, gc_tmt);
    }

    static int create(lua_State *L) {
        TinyMT32 *o = new TinyMT32();
        set_seed(o, luaL_optinteger(L, 1, 0));        
        lua_boxpointer(L, o);
        luaL_getmetatable(L, className);
        lua_setmetatable(L, -2);
        return 1;
    }
};

class LuaXoshiro128 {
    static const char className[];
    static const luaL_reg methods[];

    static Xoshiro128Ctx *checkInstance(lua_State *L, int narg) {
        luaL_checktype(L, narg, LUA_TUSERDATA);
        void *ud = luaL_checkudata(L, narg, className);
        if(!ud) luaL_typerror(L, narg, className);
        return *(Xoshiro128Ctx**)ud;  // unbox pointer
    }

    static int seed(lua_State* l) {
        Xoshiro128Ctx *o = checkInstance(l, 1);
        uint32_t next_seed = (uint32_t)luaL_checkinteger(l, 2);
        xoshiro128_seed(o, next_seed);
        return 0;
    }

    static int jump(lua_State* l) {
        Xoshiro128Ctx *o = checkInstance(l, 1);
        xoshiro128_jump(o);
        return 0;
    }

    static int number(lua_State* l) {
        Xoshiro128Ctx *o = checkInstance(l, 1);
        lua_pushinteger(l, xoshiro128_next(o));
        return 1;
    }

    static int range(lua_State* l) {
        Xoshiro128Ctx *o = checkInstance(l, 1);
        uint32_t min = (uint32_t)luaL_checkinteger(l, 2);
        uint32_t max = (uint32_t)luaL_checkinteger(l, 3);

        if (min == max)
        {
            return fixed_value(l, min);
        }

        if (min > max)
        {
            return range_error(l);
        }

        uint32_t range = max - min + 1;
        lua_pushinteger(l, min + (xoshiro128_next(o) % range));
        return 1;
    }

    static int double_num(lua_State* l) {
        Xoshiro128Ctx *o = checkInstance(l, 1);
        // Maps 32-bit unsigned integer cleanly to [0, 1) float interval
        double val = (xoshiro128_next(o) >> 8) * (1.0 / 16777216.0);
        lua_pushnumber(l, val);
        return 1;
    }

    static int double_range(lua_State* l) {
        Xoshiro128Ctx *o = checkInstance(l, 1);
        double min = luaL_checknumber(l, 2);
        double max = luaL_checknumber(l, 3);

        if (min == max)
        {
            return fixed_value(l, min);
        }

        if (min > max)
        {
            return range_error(l);
        }

        double factor = (xoshiro128_next(o) >> 8) * (1.0 / 16777216.0);
        lua_pushnumber(l, min + factor * (max - min));
        return 1;
    }

    static int toss(lua_State* l) {
        Xoshiro128Ctx *o = checkInstance(l, 1);
        lua_pushinteger(l, xoshiro128_next(o) & 1);
        return 1;
    }

    static int roll(lua_State* l) {
        Xoshiro128Ctx *o = checkInstance(l, 1);
        lua_pushinteger(l, 1 + (xoshiro128_next(o) % 6));
        return 1;
    }

    static int gc(lua_State* l) {
        // Lua's garbage collection manages the userdata lifespans natively
        return 0;
    }

    public:
    static int create(lua_State *L) {
        Xoshiro128Ctx *o = (Xoshiro128Ctx*)lua_newuserdata(L, sizeof(Xoshiro128Ctx));
        lua_boxpointer(L, o);
        luaL_getmetatable(L, className);
        lua_setmetatable(L, -2);

        uint32_t initial_seed;
        if (lua_gettop(L) > 4 && !lua_isnil(L, 1) && !lua_isnil(L, 2) && !lua_isnil(L, 3) && !lua_isnil(L, 4)) {
            uint32_t s2, s3, s4;
            initial_seed = (uint32_t)luaL_checkinteger(L, 1);
            s2 = (uint32_t)luaL_checkinteger(L, 2);
            s3 = (uint32_t)luaL_checkinteger(L, 3);
            s4 = (uint32_t)luaL_checkinteger(L, 4);
            o->s[0] = initial_seed;
            o->s[1] = s2;
            o->s[2] = s3;
            o->s[3] = s4;                        
        } else if (lua_gettop(L) > 1 && !lua_isnil(L, 1)) {
            initial_seed = (uint32_t)luaL_checkinteger(L, 1);            
            xoshiro128_seed(o, initial_seed);
        } else {
            // Fallback placeholder for entropy seed implementation
            initial_seed = (uint32_t)(time(NULL) ^ rand());
            xoshiro128_seed(o, initial_seed);
        }

        return 1;
    }

    static void Register(lua_State *l) {
        RegisterRng(l, className, methods, create, gc);
    }   
};

class Rng
{
    public:
    Rng() {};

    static int pcg32(lua_State *l) {
        LuaPcg32::create(l);
        return 1;
    }

    static int tinymt32(lua_State *l) {
        LuaTinyMT32::create(l);
        return 1;
    }

    static int xoshiro128(lua_State *l) {
        LuaXoshiro128::create(l);
        return 1;
    }        
};

static const struct luaL_reg rnglib [] =
{
    { "pcg32", Rng::pcg32 },
    { "tinymt32", Rng::tinymt32 },
    { "xoshiro128", Rng::xoshiro128 },
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

const char LuaTinyMT32::className[] = "TinyMT32";

const luaL_reg LuaTinyMT32::methods[] = {
    method(LuaTinyMT32, seed),
    method(LuaTinyMT32, number),
    { "double", LuaTinyMT32::double_num },
    method(LuaTinyMT32, range),
    method(LuaTinyMT32, double_range),
    method(LuaTinyMT32, roll),
    method(LuaTinyMT32, toss),
    {0,0}
};

const char LuaXoshiro128::className[] = "Xoshiro128";

const luaL_reg LuaXoshiro128::methods[] = {
    method(LuaXoshiro128, seed),
    method(LuaXoshiro128, number),
    { "double", LuaXoshiro128::double_num },
    method(LuaXoshiro128, range),
    method(LuaXoshiro128, double_range),
    method(LuaXoshiro128, roll),
    method(LuaXoshiro128, toss),
    method(LuaXoshiro128, jump),
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
    LuaPcg32::Register(params->m_L);
    LuaTinyMT32::Register(params->m_L);
    LuaXoshiro128::Register(params->m_L);
        
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