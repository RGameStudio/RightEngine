#pragma once

#include "VertexArray.hpp"
#include "Material.hpp"
#include "EnvironmentMapLoader.hpp"
#include <glm/glm.hpp>

namespace RightEngine
{
    struct TagComponent
    {
        TagComponent(const std::string& name, uint32_t id = 0) : name(name), id(id)
        {}

        TagComponent()
        {}

        std::string name{ "Entity" };
        uint32_t id;
    };

    // TODO: Add dirty flag, so we can recalculate transform only when needed
    class TransformComponent
    {
    public:
        inline const glm::vec3& GetWorldPosition() const
        { return transformMatrix[3]; }

        inline glm::vec3& GetLocalPosition()
        { return position; }

        void SetPosition(const glm::vec3& newPosition)
        { position = newPosition; }

        glm::vec3& GetRotation()
        { return rotation; }

        void SetRotation(const glm::vec3& newRotation)
        { rotation = newRotation; }

        glm::vec3& GetScale()
        { return scale; }

        void SetScale(const glm::vec3& newScale)
        { scale = newScale; }

        glm::mat4 GetLocalTransformMatrix() const;
        const glm::mat4& GetWorldTransformMatrix() const;

        void RecalculateTransform(TransformComponent& parentTransform);
        void RecalculateTransform();

    private:
        glm::vec3 position{ 0.0f, 0.0f, 0.0f };
        glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
        glm::mat4 transformMatrix{ glm::mat4(1.0f) };
    };

    class MeshComponent
    {
    public:
        MeshComponent();

        const std::shared_ptr<VertexArray>& GetVertexArray() const;
        void SetVertexArray(std::shared_ptr<VertexArray>& newVertexArray);

        const std::shared_ptr<Material>& GetMaterial() const;
        void SetMaterial(const std::shared_ptr<Material>& newMaterial);

        inline bool IsVisible() const
        { return isVisible; }

        void SetVisibility(bool aIsVisible)
        { isVisible = aIsVisible; }

    private:
        std::shared_ptr<VertexArray> vertexArray;
        std::shared_ptr<Material> material;
        bool isVisible{ true };
    };

    enum class LightType
    {
        DIRECTIONAL = 0
    };

    struct LightComponent
    {
        LightType type{ LightType::DIRECTIONAL };
        glm::vec3 color{ 1.0f, 1.0f, 1.0f };
        float intensity{ 1000.0f };
    };

    enum class SkyboxType
    {
        CUBE = 0
    };

    struct SkyboxComponent
    {
        SkyboxType type;
        std::shared_ptr<EnvironmentContext> environment;
    };
}
