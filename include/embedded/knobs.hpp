#ifndef LUADIO_KNOBS_HPP
#define LUADIO_KNOBS_HPP

#include <cstdint>
#include <cstdlib>

namespace luadio
{
	class knobs
	{
	public:
		static size_t get_size();
		static uint8_t *get_data();
	};
}

#endif