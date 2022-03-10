#include "Input.hpp"
#include "EventDispatcher.hpp"
#include "KeyEvent.hpp"
#include <GLFW/glfw3.h>
#include <cstring>

float RightEngine::Input::lastFrame = 0.0f;
float RightEngine::Input::deltaTime = 0.0f;
float RightEngine::Input::frameTime = 0.0f;
bool RightEngine::Input::keyArray[512];

bool RightEngine::Input::IsKeyDown(int keyCode)
{
    return keyArray[keyCode];
}

bool RightEngine::Input::IsKeyUp(int keyCode)
{
    return !keyArray[keyCode];
}

RightEngine::Input& RightEngine::Input::Get()
{
    static Input input;
    return input;
}

void RightEngine::Input::Init()
{
    Input::Get();
}

RightEngine::Input::Input()
{
    std::memset(keyArray, 0, sizeof(keyArray));
    EventDispatcher::Get().Subscribe(KeyPressedEvent::descriptor, EVENT_CALLBACK(Input::OnEvent));
    EventDispatcher::Get().Subscribe(KeyReleasedEvent::descriptor, EVENT_CALLBACK(Input::OnEvent));
}

float RightEngine::Input::GetTime()
{
    return glfwGetTime();
}

void RightEngine::Input::OnUpdate()
{
    float currentFrame = GetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    frameTime = deltaTime * 1000;
}

bool RightEngine::Input::OnEvent(const Event& KeyEvent)
{
    if (KeyEvent.GetType() == KeyPressedEvent::descriptor)
    {
        const auto event = static_cast<const KeyPressedEvent&>(KeyEvent);
        keyArray[event.GetKeyCode()] = true;
    }
    if (KeyEvent.GetType() == KeyReleasedEvent::descriptor)
    {
        const auto event = static_cast<const KeyReleasedEvent&>(KeyEvent);
        keyArray[event.GetKeyCode()] = false;
    }
}
