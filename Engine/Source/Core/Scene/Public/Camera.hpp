#pragma once

#include "MouseEvent.hpp"
#include "KeyEvent.hpp"
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

namespace RightEngine
{
    class Camera
    {
    public:
        Camera(const glm::vec3& position, const glm::vec3& worldUp);
        ~Camera();

        virtual void OnUpdate();

        glm::mat4 GetViewMatrix() const;

        const glm::vec3& GetPosition() const;
        const glm::vec3& GetRotation() const;
        const glm::vec3& GetFront() const;

        float GetMovementSpeed() const;
        void SetMovementSpeed(float aSpeed)
        { movementSpeed = aSpeed; }

        inline float GetFOV(bool asRadians = false) const
        {
            if (asRadians)
            {
                return glm::radians(fov);
            }
            return fov;
        }
        inline float GetAspectRatio() const
        { return aspectRatio; }
        inline float GetNear() const
        { return zNear; }
        inline float GetFar() const
        { return zFar; }

        void SetFOV(float aFOV)
        { fov = aFOV; }
        void SetAspectRatio(float aAspectRatio)
        { aspectRatio = aAspectRatio; }
        void SetNear(float aNear)
        { zNear = aNear; }
        void SetFar(float aFar)
        { zFar = aFar; }

    protected:
        glm::vec3 position;
        glm::vec3 up;
        glm::vec3 front;
        glm::vec3 right;
        glm::vec3 worldUp;
        glm::vec3 rotation;

        float zNear{ 0.1f };
        float zFar{ 500.0f };
        float aspectRatio{ 16.0f / 9.0f };
        float fov{ 50.0f };
        float movementSpeed{ 40.0f };
        float sensitivity{ 0.25f };
        float actualSpeed{ 1.0f };
        float prevXMousePos{ -1 };
        float prevYMousePos{ -1 };

        bool active{ true };
        bool invertY{ false };

    private:
        void UpdateVectors();
        bool OnEvent(const Event& event);
        bool OnMouseMove(const MouseMovedEvent& e);
        void Move();
    };
}