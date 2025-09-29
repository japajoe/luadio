#include "texture_2d.hpp"
#include "../external/glad/glad.h"
#include "image.hpp"
#include <utility>
#include <stdexcept>

namespace luadio
{
	texture_2d::texture_2d()
	{
		id = 0;
		width = 0;
		height = 0;
	}

	texture_2d::texture_2d(const texture_2d &other)
	{
		id = other.id;
		width = other.width;
		height = other.height;
	}

	texture_2d::texture_2d(texture_2d &&other) noexcept
	{
		id = std::exchange(other.id, 0);
		width = std::exchange(other.width, 0);
		height = std::exchange(other.height, 0);
	}

	texture_2d &texture_2d::operator=(const texture_2d &other)
	{
		if(this != &other)
		{
			id = other.id;
			width = other.width;
			height = other.height;
		}
		return *this;
	}

	texture_2d &texture_2d::operator=(texture_2d &&other) noexcept
	{
		if(this != &other)
		{
			id = std::exchange(other.id, 0);
			width = std::exchange(other.width, 0);
			height = std::exchange(other.height, 0);
		}
		return *this;
	}

	void texture_2d::generate(const image *img)
	{
		generate(img->get_data(), img->get_data_size(), img->get_width(), img->get_height(), img->get_channels());
	}

	void texture_2d::generate(const uint8_t *data, size_t size, uint32_t width, uint32_t height, uint32_t channels)
	{
		if(data != nullptr)
		{
			this->width = width;
			this->height = height;

			if(!id)
				glGenTextures(1, &id);

			glBindTexture(GL_TEXTURE_2D, id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			switch(channels)
			{
				case 1:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);    
					break;
				}
				case 2:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
					break;
				}
				case 3:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					break;
				}
				case 4:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					break;
				}
				default:
				{
					glBindTexture(GL_TEXTURE_2D, 0);
					glDeleteTextures(1, &id);
					id = 0;
					std::string error = "Failed to load texture: Unsupported number of channels: " + std::to_string(channels);
					throw std::invalid_argument(error);
				}
			}
			
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			throw std::invalid_argument("Failed to generate texture: data can not be null");
		}
	}

	void texture_2d::destroy()
	{
		if(id)
			glDeleteTextures(1, &id);

		id = 0;
		width = 0;
		height = 0;
	}

	void texture_2d::bind(uint32_t unit)
	{
		if(id)
		{
			glActiveTexture(GL_TEXTURE0 + unit);
			glBindTexture(GL_TEXTURE_2D, id);
		}
	}

	void texture_2d::unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	uint32_t texture_2d::get_id() const
	{
		return id;
	}

	uint32_t texture_2d::get_width() const
	{
		return width;
	}

	uint32_t texture_2d::get_height() const
	{
		return height;
	}
}