#pragma once

#include "MouseEvent.hpp"
#include "KeyEvent.hpp"
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

namespace RightEngine
{
    class FPSCamera
    {
    public:
        FPSCamera(const glm::vec3& position, const glm::vec3& worldUp);

        ~FPSCamera() = default;

        void OnUpdate();

        glm::mat4 GetViewMatrix() const;

        const glm::vec3& GetPosition() const;

        const glm::vec3& GetRotation() const;

        const glm::vec3& GetFront() const;

        const float& GetCameraMovementSpeed() const;

        void ToggleMouseCapture();

        bool IsMouseCaptured();

    private:
        glm::vec3 position;
        glm::vec3 up;
        glm::vec3 front;
        glm::vec3 right;
        glm::vec3 worldUp;
        glm::vec3 rotation;

        float movementSpeed{ 25.0f };
        float sensitivity{ 0.25f };
        float actualSpeed{ 1.0f };
        float prevXMousePos{ -1 };
        float prevYMousePos{ -1 };

        bool shouldCaptureMouse = true;

        void UpdateVectors();

        bool OnEvent(const Event& event);

        bool OnMouseMove(const MouseMovedEvent& e);

        bool OnKeyPressed(const KeyPressedEvent& e);
    };
}