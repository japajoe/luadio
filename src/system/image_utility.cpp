#include "image_utility.hpp"
#include <cstring>
#include <cmath>
#include <cstdlib>

namespace luadio
{
	image *image_utility::create_dot_matrix(int width, int height, int dotSize, int spacing, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		if (width <= 0 || height <= 0) 
			return nullptr;

		if (spacing <= 0 || dotSize <= 0) 
			return nullptr;

		const int channels = 4;
		size_t bufSize = (size_t)width * (size_t)height * channels;
		uint8_t* buf = new (std::nothrow) uint8_t[bufSize];
		
		if (!buf) 
			return nullptr;
		
		std::memset(buf, 0, bufSize); // transparent background

		int half = dotSize / 2;
		int start_off = spacing / 2; // first center at spacing/2

		for (int cy = start_off; cy < height; cy += spacing)
		{
			int y0 = cy - half;
			int y1 = y0 + dotSize - 1;
			
			if (y1 < 0 || y0 >= height) 
				continue;
			
			if (y0 < 0) 
				y0 = 0;
			
			if (y1 >= height) 
				y1 = height - 1;

			for (int cx = start_off; cx < width; cx += spacing)
			{
				int x0 = cx - half;
				int x1 = x0 + dotSize - 1;
				if (x1 < 0 || x0 >= width) 
					continue;
				
				if (x0 < 0) 
					x0 = 0;
				
				if (x1 >= width) 
					x1 = width - 1;

				for (int y = y0; y <= y1; ++y)
				{
					size_t rowBase = (size_t)y * (size_t)width * channels;
					for (int x = x0; x <= x1; ++x)
					{
						size_t idx = rowBase + (size_t)x * channels;
						buf[idx + 0] = r;
						buf[idx + 1] = g;
						buf[idx + 2] = b;
						buf[idx + 3] = a;
					}
				}
			}
		}

		image *pImage = new image(buf, bufSize, width, height, channels);

		delete[] buf;

		return pImage;
	}
}