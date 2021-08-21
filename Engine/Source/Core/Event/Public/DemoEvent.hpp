#pragma once

#include "Event.hpp"

class DemoEvent : public Event
{
public:
    static constexpr const char* descriptor = "DemoEvent";

    virtual const char* GetType() const override
    {
        return descriptor;
    }
};