#include "Camera.hpp"
#include "Input.hpp"
#include "EventDispatcher.hpp"
#include "KeyCodes.hpp"
#include "Logger.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace RightEngine;

Camera::Camera(const glm::vec3& position, const glm::vec3& worldUp) : position(position),
                                                                      worldUp(worldUp),
                                                                      rotation(90.0f, 0.0f, 0.0f)
{
    UpdateVectors();
}

Camera::~Camera()
{
}

void Camera::OnUpdate(float deltaTime)
{
    actualSpeed = movementSpeed * deltaTime;
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

const glm::vec3& Camera::GetPosition() const
{
    return position;
}

const glm::vec3& Camera::GetRotation() const
{
    return rotation;
}

const glm::vec3& Camera::GetFront() const
{
    return front;
}

float Camera::GetMovementSpeed() const
{
    return movementSpeed;
}

void Camera::UpdateVectors()
{
    float yaw = rotation[0];
    float pitch = rotation[1];
    glm::vec3 rotatedFront;
    rotatedFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    rotatedFront.y = sin(glm::radians(pitch));
    rotatedFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(rotatedFront);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::Move(int keycode)
{
    if (keycode == R_KEY_W)
    {
        position += actualSpeed * front;
    }
    if (keycode == R_KEY_S)
    {
        position -= actualSpeed * front;
    }
    if (keycode == R_KEY_A)
    {
        position -= glm::normalize(glm::cross(front, up)) * actualSpeed;
    }
    if (keycode == R_KEY_D)
    {
        position += glm::normalize(glm::cross(front, up)) * actualSpeed;
    }
}

void Camera::Rotate(float x, float y)
{
    if (!active)
    {
        prevXMousePos = x;
        prevYMousePos = y;
        return;
    }
    if (prevXMousePos == -1)
    {
        prevXMousePos = x;
        prevYMousePos = y;
    }
    float xOffset = x - prevXMousePos;
    float yOffset = prevYMousePos - y;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    rotation[0] += xOffset;
    if (invertY)
    {
        rotation[1] -= yOffset;
    }
    else
    {
        rotation[1] += yOffset;
    }

    if (rotation[1] > 89.0f)
    {
        rotation[1] = 89.0f;
    }
    if (rotation[1] < -89.0f)
    {
        rotation[1] = -89.0f;
    }
    UpdateVectors();
    prevXMousePos = x;
    prevYMousePos = y;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);
}
