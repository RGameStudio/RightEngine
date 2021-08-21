#pragma once

#include <functional>
#include <unordered_map>
#include "Event.hpp"

class EventDispatcher
{
public:

    using Callback = std::function<void(const Event &)>;

    void Subscribe(const char *descriptor, Callback &&slot);

    void Emit(const Event &event) const;

    inline static EventDispatcher *Get()
    {
        if (!instance)
        {
            instance = new EventDispatcher();
        }
        return instance;
    }

    inline static void Destroy()
    {
        delete instance;
    }

private:

    EventDispatcher() = default;

    std::unordered_map<const char *, std::vector<Callback>> observers;

    static EventDispatcher *instance;
};
