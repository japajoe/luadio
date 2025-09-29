#include "timer.hpp"

namespace luadio
{
	timer::timer()
	{
		tp1 = std::chrono::system_clock::now();
		tp1 = std::chrono::system_clock::now();
		deltaTime = 0;
		elapsedTime = 0;
	}

	void timer::update()
	{
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsed = tp2 - tp1;
		tp1 = tp2;
		deltaTime = elapsed.count();
		elapsedTime += deltaTime;
	}
}