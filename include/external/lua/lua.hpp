#ifndef LUA_HPP
#define LUA_HPP

//#include <luajit-2.1/lua.hpp>

extern "C" {
#include "../../libs/LuaJIT-cmake/src/lua.h"
#include "../../libs/LuaJIT-cmake/src/lualib.h"
#include "../../libs/LuaJIT-cmake/src/lauxlib.h"
#include "../../libs/LuaJIT-cmake/src/luajit.h"

//COMPAT53_API functions
LUALIB_API int lua_getfield_with_type(lua_State *L, int idx, const char *k);
LUALIB_API int luaL_getsubtable (lua_State *L, int i, const char *name) ;
LUALIB_API int lua_absindex(lua_State *L, int i);
LUALIB_API void luaL_requiref(lua_State *L, const char *modname, lua_CFunction openf, int glb);

}

#endif