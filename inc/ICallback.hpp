#pragma once

class ICallback
{
public:
    virtual void operator()() = 0;
    virtual ~ICallback() {}
};