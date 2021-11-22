#include "Input.hpp"
#include <cmath>

using namespace std::chrono;

high_resolution_clock::time_point RightEngine::Input::launchTime = high_resolution_clock::now();
float RightEngine::Input::lastFrameTime = 0.0f;
float RightEngine::Input::lastFrameSinceStartTime = 0.0f;
float RightEngine::Input::deltaTime = 0.0f;
float RightEngine::Input::frameTime = 0.0f;

float RightEngine::Input::GetTime()
{
    return duration<float>(high_resolution_clock::now() - launchTime).count() * 1000;
}

void RightEngine::Input::OnUpdate()
{
    float currentFrameSinceStartTime = GetTime();
    float currentFrameTime = currentFrameSinceStartTime - lastFrameSinceStartTime;
    deltaTime = std::fabs(lastFrameTime - currentFrameTime);
    lastFrameTime = currentFrameTime;
    lastFrameSinceStartTime = currentFrameSinceStartTime;
    frameTime = currentFrameTime;
}
