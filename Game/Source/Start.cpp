#include "EntryPoint.hpp"

#include "DemoEvent.hpp"
#include <functional>

class TestEventHandler
{
public:
    TestEventHandler()
    {
        EventDispatcher::Get()->Subscribe(DemoEvent::descriptor, EVENT_CALLBACK(TestEventHandler::OnEvent));
    }

    bool OnEvent(const Event &e)
    {
        if (e.GetType() == DemoEvent::descriptor)
        {
            EventDispatcher::Get()->UnSubscribe(DemoEvent::descriptor, EVENT_CALLBACK(TestEventHandler::OnEvent));
            R_INFO("Got demo event!");
        }

        return true;
    }
};


void GameApplication::OnStart()
{
    TestEventHandler test;
    EventDispatcher *dispatcher = EventDispatcher::Get();

    dispatcher->Emit(DemoEvent());
    dispatcher->Emit(DemoEvent());
}

void GameApplication::OnUpdate()
{
}

void GameApplication::OnDestroy()
{
}
