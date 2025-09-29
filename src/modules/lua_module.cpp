#include "lua_module.hpp"
#include <unordered_map>

namespace luadio
{
	std::unordered_map<std::string,void*> lua_module::delegates;
	static std::unordered_map<std::string,std::string> gModules;

	void lua_module::register_method(lua_State *L, const std::string &name, lua_CFunction pFunc)
	{
		lua_register(L, name.c_str(), pFunc);
	}

	void lua_module::register_external_method(lua_State *L, const std::string &name, void *pFunc)
	{
		if(delegates.contains(name))
			return;
		delegates[name] = pFunc;
	}

	bool lua_module::register_source(lua_State *L, const std::string &source, const std::string &name)
	{
		if(gModules.contains(name))
			return false;

		gModules[name] = source;
		luaL_requiref(L, name.c_str(), openf, 0);
		return true;
	}

	int lua_module::openf(lua_State *L)
	{
		/* Get the module name as passed to luaL_requiref. */
		std::string name = lua_tostring(L, 1);

		int res;

		/*
		Check if we know the module. We can use this function to load many
		different Lua modules uniquely identified by modname.
		*/

		if(gModules.contains(name))
		{
			/*
			Parses the Lua source code and leaves the compiled function on the top
			of the stack if there are no errors.
			*/
			res = luaL_loadbufferx(L, gModules[name].c_str(), gModules[name].size(), name.c_str(), "t");
		}
		else 
		{
			/* Unknown module. */
			return lua_error(L);
		}

		/* Check if the call to luaL_loadbufferx was successful. */
		if (res != LUA_OK)
		{
			return lua_error(L);
		}

		/*
		Runs the Lua code and returns whatever it returns as the result of OpenF,
		which will be used as the value of the module.
		*/
		//lua_call(L, 0, 1);

		if (lua_pcall(L, 0, 1, 0) != LUA_OK) 
		{
			const char *error = lua_tostring(L, -1);
			printf("Error: %s\n", error);
			lua_pop(L, 1); // Remove error message from stack
			return 0;
		}

		return 1;
	}
}