#include "IntervalTimer.hpp"


IntervalTimer::IntervalTimer()
    : _interval(0), _callback(NULL)
{
}

IntervalTimer::IntervalTimer(float interval, ICallback* cb)
        : _interval(interval), _callback(cb)
        {}

IntervalTimer::~IntervalTimer()
{
}

void IntervalTimer::update()
{
    if (_timer.getDuration() >= _interval)
    {
        if (_callback)
            (*_callback)(); 
        _timer.reset();
    }
}