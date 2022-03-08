#include "EventDispatcher.hpp"

void EventDispatcher::Subscribe(const char* descriptor, CallbackContext context)
{
    observers[descriptor].push_back(context);
}

void EventDispatcher::UnSubscribe(const char* descriptor, CallbackContext context)
{
    std::vector<CallbackContext> newCallbacks;
    for (auto&& callbackContext: observers.at(descriptor))
    {
        if (callbackContext.instancePointer != context.instancePointer) {
            newCallbacks.push_back(callbackContext);
        }
    }

    observers[descriptor] = newCallbacks;
}

void EventDispatcher::Emit(const Event& event) const
{
    auto type = event.GetType();

    if (observers.find(type) == observers.end())
    {
        return;
    }

    auto&& observersList = observers.at(type);

    for (auto&& observer: observersList)
    {
        observer.callback(event);
    }
}
