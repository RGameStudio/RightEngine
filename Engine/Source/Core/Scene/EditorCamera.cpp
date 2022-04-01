#include "EditorCamera.hpp"
#include "Input.hpp"

using namespace RightEngine;

EditorCamera::EditorCamera(const glm::vec3& position, const glm::vec3& worldUp, bool invertY) : Camera(position, worldUp)
{
    this->invertY = invertY;
}

void EditorCamera::OnUpdate()
{
    if (Input::IsMouseButtonDown(MouseButton::Middle))
    {
        active = true;
        Camera::OnUpdate();
    }
    else
    {
        active = false;
    }
}
