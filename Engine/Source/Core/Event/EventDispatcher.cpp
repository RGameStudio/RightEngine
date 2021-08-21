#include "EventDispatcher.hpp"

void EventDispatcher::Subscribe(const char *descriptor, EventDispatcher::Callback &&slot)
{
    observers[descriptor].push_back(slot);
}

void EventDispatcher::Emit(const Event &event) const
{
    auto type = event.GetType();

    if (observers.find(type) == observers.end())
    {
        return;
    }

    auto&& observersList = observers.at(type);

    for (auto&& observer: observersList) {
        observer(event);
    }
}

EventDispatcher* EventDispatcher::instance = nullptr;
