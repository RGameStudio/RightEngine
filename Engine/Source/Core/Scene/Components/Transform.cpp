#include "Components.hpp"
#include <glm/ext/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace RightEngine;

glm::mat4 TransformComponent::GetLocalTransformMatrix() const
{
    const glm::mat4 rotationMatrix = glm::toMat4(glm::quat(rotation));

    return glm::translate(glm::mat4(1.0f), position) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);
}

const glm::mat4& TransformComponent::GetWorldTransformMatrix() const
{
    return transformMatrix;
}

void TransformComponent::RecalculateTransform(TransformComponent& parentTransform)
{
    const auto parentTransformMatrix = glm::translate(glm::mat4(1.0f), parentTransform.GetLocalPosition());
    transformMatrix = parentTransformMatrix * GetLocalTransformMatrix();
}

void TransformComponent::RecalculateTransform()
{
    transformMatrix = GetLocalTransformMatrix();
}

