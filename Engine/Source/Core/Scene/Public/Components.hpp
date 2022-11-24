#pragma once

#include "Material.hpp"
#include "EnvironmentMapLoader.hpp"
#include <glm/glm.hpp>
#include <crossguid/guid.hpp>

namespace RightEngine
{
    struct TagComponent
    {
        TagComponent(const std::string& name, uint32_t id = 0) : name(name), id(id)
        {}

        TagComponent()
        {}

        std::string name{"Entity"};
        uint32_t id;
        xg::Guid guid{xg::newGuid()};
    };

    // TODO: Add dirty flag, so we can recalculate transform only when needed
    class TransformComponent
    {
    public:
        inline const glm::vec3 GetWorldPosition() const
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
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::vec3 rotation{0.0f, 0.0f, 0.0f};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::mat4 transformMatrix{glm::mat4(1.0f)};
    };

    class MeshComponent
    {
    public:
        MeshComponent()
        { material = std::make_shared<Material>(); }

        void SetVisibility(bool aVisible)
        { isVisible = aVisible; }

        const bool& IsVisible() const
        { return isVisible; }

        const std::shared_ptr<Material>& GetMaterial() const
        { return material; }

        void SetMaterial(const std::shared_ptr<Material>& aMaterial)
        { material = aMaterial; }

        const AssetHandle& GetMesh() const
        { return mesh; }

        void SetMesh(const AssetHandle& handle)
        { mesh = handle; }

    private:
        AssetHandle mesh;
        //TODO: Make material a separate asset
        std::shared_ptr<Material> material;
        bool isVisible;
    };

    enum class LightType
    {
        DIRECTIONAL = 0
    };

    struct LightComponent
    {
        LightType type{LightType::DIRECTIONAL};
        glm::vec3 color{1.0f, 1.0f, 1.0f};
        float intensity{1000.0f};
    };

    enum class SkyboxType
    {
        CUBE = 0
    };

    struct SkyboxComponent
    {
        SkyboxType type;
        AssetHandle environmentHandle;
        bool isDirty{true};
    };
}
