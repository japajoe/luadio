#ifndef LUADIO_WAVETABLE_MODULE_HPP
#define LUADIO_WAVETABLE_MODULE_HPP

#include "lua_module.hpp"

namespace luadio
{
	class wavetable_module : public lua_module
	{
	public:
		void load(lua_State *L) override;
	};
}

#endif