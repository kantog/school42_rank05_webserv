#pragma once

#include <ctime>

class Timer
{
public:
    Timer();
    ~Timer();
    double getDuration() const; 
    void reset();

private:
    std::time_t _start;
};