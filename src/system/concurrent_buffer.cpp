#include "concurrent_buffer.hpp"
#include <cstring>
#include <stdexcept>

namespace luadio
{
	concurrent_buffer::concurrent_buffer()
	{
		size_t capacity = 1024;
		buffer.resize(capacity);
		currentLength = 0;
	}

	concurrent_buffer::concurrent_buffer(size_t capacityPowerOfTwo)
	{
		if (capacityPowerOfTwo <= 0 || (capacityPowerOfTwo & (capacityPowerOfTwo - 1)) != 0)
			throw std::invalid_argument("capacityPowerOfTwo must be power of two");
		size_t capacity = capacityPowerOfTwo;
		buffer.resize(capacity);
		currentLength = 0;
	}

	size_t concurrent_buffer::write(const float *pSrc, size_t length)
	{
		std::lock_guard<std::mutex> lock(mutex);
		resize(length);
		std::memcpy(buffer.data(), pSrc, length * sizeof(float));
		currentLength = length;
		return currentLength;
	}

	size_t concurrent_buffer::read(std::vector<float> &output)
	{
		std::lock_guard<std::mutex> lock(mutex);
		
		if (output.size() < buffer.size())
			output.resize(buffer.size());

		std::memcpy(output.data(), buffer.data(), buffer.size() * sizeof(float));
		return currentLength;
	}

	void concurrent_buffer::resize(size_t capacity)
	{
		if(buffer.size() < capacity)
		{
			size_t currentSize = buffer.size();

			while(currentSize < capacity)
			{
				buffer.resize(currentSize * 2);
				currentSize = buffer.size();
			}
		}
	}
}