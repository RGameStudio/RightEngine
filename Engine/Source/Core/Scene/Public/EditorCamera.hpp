#pragma once

#include "Camera.hpp"

namespace RightEngine
{
    class EditorCamera : public Camera
    {
    public:
        EditorCamera(const glm::vec3& position, const glm::vec3& worldUp, bool invertY = true);

        virtual void OnUpdate() override;

    };
}
