#pragma once

class Event
{
public:
    virtual ~Event() = default;

    virtual const char* GetType() const = 0;
};