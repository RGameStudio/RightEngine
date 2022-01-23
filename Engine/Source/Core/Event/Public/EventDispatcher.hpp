#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include "Event.hpp"

using Callback = std::function<bool(const Event&)>;

struct CallbackContext
{
    Callback callback;
    void* instancePointer = nullptr;

    CallbackContext(Callback&& callback, void* instancePointer) : callback(callback), instancePointer(instancePointer)
    {}
};

class EventDispatcher
{
public:

    void Subscribe(const char* descriptor, CallbackContext context);

    void UnSubscribe(const char* descriptor, CallbackContext context);

    void Emit(const Event& event) const;

    inline static EventDispatcher* Get()
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

    std::unordered_map<const char*, std::vector<CallbackContext>> observers;

    static EventDispatcher* instance;
};

#define EVENT_CALLBACK(callback) CallbackContext(std::bind(&callback, this, std::placeholders::_1), this)
