#ifndef LUADIO_TIMER_HPP
#define LUADIO_TIMER_HPP

#include <chrono>
namespace luadio
{
    typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> time_point;

    struct timer
    {
        time_point tp1;
        time_point tp2;
        float deltaTime;
		double elapsedTime;
		timer();
        void update();
    };
}

#endif