#pragma once

#include "Camera.hpp"

namespace RightEngine
{
    class EditorCamera : public Camera
    {
    public:
        EditorCamera(const glm::vec3& position, const glm::vec3& worldUp);

        virtual void OnUpdate() override;

    };
}
