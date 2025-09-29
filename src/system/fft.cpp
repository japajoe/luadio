#include "fft.hpp"

namespace luadio
{
	// Assumes n is a power of 2
	void fft::perform(std::vector<std::complex<double>> &buffer, size_t n)
	{
		// Bit-reversal permutation
		int j = 0;

		for (int i = 1; i < n; ++i)
		{
			int bit = n >> 1;
			while (j & bit)
			{
				j ^= bit;
				bit >>= 1;
			}
			j ^= bit;
			if (i < j)
				std::swap(buffer[i], buffer[j]);
		}

		// Cooley-Tukey FFT
		for (int len = 2; len <= n; len <<= 1)
		{
			double ang = -2 * M_PI / len;
			std::complex<double> wlen(std::cos(ang), std::sin(ang));
			for (int i = 0; i < n; i += len)
			{
				std::complex<double> w(1);
				for (int j = 0; j < len / 2; ++j)
				{
					std::complex<double> u = buffer[i + j];
					std::complex<double> v = buffer[i + j + len / 2] * w;
					buffer[i + j] = u + v;
					buffer[i + j + len / 2] = u - v;
					w *= wlen;
				}
			}
		}
	}
}