#ifndef LUADIO_OSCILLATOR_MODULE_HPP
#define LUADIO_OSCILLATOR_MODULE_HPP

#include "lua_module.hpp"

namespace luadio
{
	class oscillator_module : public lua_module
	{
	public:
		void load(lua_State *L) override;
	};
}

#endif