#ifndef LUADIO_TEXTURE_2D_HPP
#define LUADIO_TEXTURE_2D_HPP

#include <cstdlib>
#include <cstdint>

namespace luadio
{
	class image;

	class texture_2d
	{
	public:
		texture_2d();
		texture_2d(const texture_2d &other);
		texture_2d(texture_2d &&other) noexcept;
		texture_2d &operator=(const texture_2d &other);
		texture_2d &operator=(texture_2d &&other) noexcept;
		void generate(const image *img);
		void generate(const uint8_t *data, size_t size, uint32_t width, uint32_t height, uint32_t channels);
		void destroy();
		void bind(uint32_t unit);
		void unbind();
		uint32_t get_id() const;
		uint32_t get_width() const;
		uint32_t get_height() const;
	private:
		uint32_t id;
		uint32_t width;
		uint32_t height;
	};
}

#endif