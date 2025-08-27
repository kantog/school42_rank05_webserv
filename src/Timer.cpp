#include "Timer.hpp"

Timer::Timer()
{
    reset();
}

Timer::~Timer()
{
}

double Timer::getDuration() const
{
    return std::difftime(std::time(NULL), _start);
}

void Timer::reset()
{
    _start = std::time(NULL);
}