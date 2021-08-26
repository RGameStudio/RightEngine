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

    void OnEvent(const Event &e)
    {
        if (e.GetType() == DemoEvent::descriptor)
        {
            EventDispatcher::Get()->UnSubscribe(DemoEvent::descriptor, EVENT_CALLBACK(TestEventHandler::OnEvent));
            R_INFO("Got demo event!");
        }
    }
};


void GameApplication::OnStart()
{
    TestEventHandler test;
    EventDispatcher *dispatcher = EventDispatcher::Get();

    dispatcher->Emit(DemoEvent());
    dispatcher->Emit(DemoEvent());
    GShouldStop = true;
}

void GameApplication::OnUpdate()
{
}

void GameApplication::OnDestroy()
{
}
