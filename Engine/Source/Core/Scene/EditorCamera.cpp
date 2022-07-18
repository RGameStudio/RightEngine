#include "EditorCamera.hpp"
#include "Input.hpp"

using namespace RightEngine;

EditorCamera::EditorCamera(const glm::vec3& position, const glm::vec3& worldUp, bool invertY) : Camera(position, worldUp)
{
    this->invertY = invertY;
}

void EditorCamera::OnUpdate()
{
    if (Input::IsMouseButtonDown(MouseButton::Right))
    {
        active = true;
        Camera::OnUpdate();
    }
    else
    {
        active = false;
    }

    const auto mouseDelta = Input::GetMouseDelta();
    if (glm::abs(mouseDelta.y) >= 1.0f)
    {
        fov -= mouseDelta.y;
    }
    fov = glm::clamp(fov, 30.0f, 90.0f);
}
