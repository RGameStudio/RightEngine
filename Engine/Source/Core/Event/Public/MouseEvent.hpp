#pragma once

#include "Event.hpp"

class MouseMovedEvent : public Event
{
public:
    EVENT_BODY("MouseMovedEvent")

    MouseMovedEvent(float x, float y) : mouseX(x), mouseY(y)
    {}

    inline const float& GetX() const
    { return mouseX; }

    inline const float& GetY() const
    { return mouseY; }

private:
    float mouseX, mouseY;
};

