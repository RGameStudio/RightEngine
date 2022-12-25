#pragma once

#include "Material.hpp"
#include "EnvironmentMapLoader.hpp"
#include <glm/glm.hpp>
#include <crossguid/guid.hpp>

namespace RightEngine
{
    struct TagComponent
    {
        TagComponent(const std::string& name) : name(name)
        {}

        TagComponent()
        {}

        std::string name;
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

        void SetRotationDegree(const glm::vec3& newRotation)
        { rotation = glm::radians(newRotation); }

        void SetRotationRadians(const glm::vec3& newRotation)
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
        // In radians
        glm::vec3 rotation{0.0f, 0.0f, 0.0f};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::mat4 transformMatrix{glm::mat4(1.0f)};
    };

    class MeshComponent
    {
    public:
        MeshComponent();

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

        void SetDirty(bool dirty)
        { isDirty = dirty; }

        bool IsDirty() const
        { return isDirty; }

    private:
        AssetHandle mesh;
        //TODO: Make material a separate asset
        std::shared_ptr<Material> material;
        bool isVisible;
        bool isDirty;
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

    class CameraComponent
    {
    public:
        CameraComponent(const glm::vec3& worldUp);
        CameraComponent();
        ~CameraComponent();

        void OnUpdate(float dt);
        // Rotation must be in degrees
        glm::vec3 Rotate(float x, float y, const glm::vec3& oldRotation);
        glm::vec3 Rotate(const glm::vec3& newRotation);
        glm::vec3 Move(int keycode, const glm::vec3& oldPosition);

        glm::mat4 GetViewMatrix(const glm::vec3& position) const;
        glm::mat4 GetProjectionMatrix() const;

        float GetMovementSpeed() const
        { return movementSpeed; }
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
        void SetActive(bool aIsActive)
        { isActive = aIsActive; }
        void SetPrimary(bool aIsPrimary)
        { isPrimary = aIsPrimary; }

        bool IsActive() const
        { return isActive; }
        bool IsPrimary() const
        { return isPrimary; }

    private:
        void UpdateVectors(const glm::vec3& rotation);

        glm::vec3 front;
        glm::vec3 worldUp;
        glm::vec3 up;
        float zNear{ 0.1f };
        float zFar{ 500.0f };
        float aspectRatio{ 16.0f / 9.0f };
        float fov{ 50.0f };
        float movementSpeed{ 40.0f };
        float sensitivity{ 0.25f };
        float actualSpeed{ 1.0f };
        float prevXMousePos{ -1 };
        float prevYMousePos{ -1 };
        float positionDelta{ 0.0f };

        bool isActive{ false };
        bool isPrimary{ false };
    };
}
