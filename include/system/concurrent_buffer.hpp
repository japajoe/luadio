#ifndef LUADIO_CONCURRENT_BUFFER_HPP
#define LUADIO_CONCURRENT_BUFFER_HPP

#include <vector>
#include <mutex>
#include <cstdlib>

namespace luadio
{
	class concurrent_buffer
	{
	public:
		concurrent_buffer();
		concurrent_buffer(size_t capacityPowerOfTwo);
		size_t write(const float *pSrc, size_t length);
		size_t read(std::vector<float> &output);
	private:
		std::vector<float> buffer;
		std::mutex mutex;
		size_t currentLength;
		void resize(size_t capacity);
	};
}

#endif