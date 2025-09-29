#ifndef LUADIO_LUA_MODULE_HPP
#define LUADIO_LUA_MODULE_HPP

#include "../external/lua/lua.hpp"
#include <string>
#include <unordered_map>

namespace luadio
{
	class lua_module
	{
	public:
		virtual void load(lua_State *L) = 0;
	protected:
		static std::unordered_map<std::string,void*> delegates;
		void register_method(lua_State *L, const std::string &name, lua_CFunction pFunc);
		void register_external_method(lua_State *L, const std::string &name, void *pFunc);
		bool register_source(lua_State *L, const std::string &source, const std::string &name);
	private:
		static int openf(lua_State *L);
	};
}

#endif