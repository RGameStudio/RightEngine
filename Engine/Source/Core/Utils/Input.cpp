#include "Input.hpp"
#include <GLFW/glfw3.h>

float RightEngine::Input::lastFrame = 0.0f;
float RightEngine::Input::deltaTime = 0.0f;
float RightEngine::Input::frameTime = 0.0f;

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
