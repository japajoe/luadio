#ifndef LUADIO_FFT_HPP
#define LUADIO_FFT_HPP

#include <cstdlib>
#include <vector>
#include <complex>

namespace luadio
{
	class fft
	{
	public:
		static void perform(std::vector<std::complex<double>> &buffer, size_t n);
	};
}

#endif