#include "EntryPoint.hpp"

#include "DemoEvent.hpp"
#include <functional>

class TestEventHandler
{
public:
    TestEventHandler()
    {
        EventDispatcher::Get()->Subscribe(DemoEvent::descriptor, std::bind(&TestEventHandler::OnEvent, this, std::placeholders::_1));
    }

    void OnEvent(const Event &e)
    {
        if (e.GetType() == DemoEvent::descriptor)
        {
            R_INFO("Got demo event!");
        }
    }
};

int Run()
{
    TestEventHandler test;
    EventDispatcher *dispatcher = EventDispatcher::Get();

    dispatcher->Emit(DemoEvent());
    return 0;
}