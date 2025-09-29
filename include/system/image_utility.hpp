#ifndef LUADIO_IMAGE_UTILITY_HPP
#define LUADIO_IMAGE_UTILITY_HPP

#include "../core/image.hpp"
#include <cstdint>

namespace luadio
{
	class image_utility
	{
	public:
		static image *create_dot_matrix(int width, int height, int dotSize, int spacing, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	};
}

#endif