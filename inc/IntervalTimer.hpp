#pragma once

#include "Timer.hpp"
#include "ICallback.hpp"

class IntervalTimer
{
public:

    IntervalTimer();
    IntervalTimer(float interval, ICallback* cb);
    ~IntervalTimer();

    void update();
private:
    Timer _timer;
    float _interval;
    ICallback* _callback;
};