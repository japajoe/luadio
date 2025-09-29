#ifndef LUADIO_LUADIO_MODULE_HPP
#define LUADIO_LUADIO_MODULE_HPP

#include "lua_module.hpp"
#include <functional>

namespace luadio
{
	using luadio_log_func = std::function<void(const char*)>;

	class luadio_module : public lua_module
	{
	public:
		static luadio_log_func onLog;
		void load(lua_State *L) override;
	private:
		static int luadio_find_function_pointer(lua_State *L);
		static void luadio_print(const char *message);
	};
}

#endif