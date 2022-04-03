#pragma once

#include <glm/glm.hpp>

namespace RightEngine
{
    // TODO: Add dirty flag, so we can recalculate transform only when needed
    class Transform
    {
    public:
        inline const glm::vec3& GetWorldPosition() const { return transformMatrix[3]; }
        inline const glm::vec3& GetLocalPosition() const { return position; }
        void SetPosition(const glm::vec3& newPosition) { position = newPosition; }

        const glm::vec3& GetRotation() const { return rotation; }
        void SetRotation(const glm::vec3& newRotation) { rotation = newRotation; }

        const glm::vec3& GetScale() const { return scale; }
        void SetScale(const glm::vec3& newScale) { scale = newScale; }

        glm::mat4 GetLocalTransformMatrix() const;
        const glm::mat4& GetWorldTransformMatrix() const;

        void RecalculateTransform(const Transform& parentTransform);
        void RecalculateTransform();

    private:
        glm::vec3 position{ 0.0f, 0.0f, 0.0f };
        glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
        glm::mat4 transformMatrix{ glm::mat4(1.0f) };
    };
}
