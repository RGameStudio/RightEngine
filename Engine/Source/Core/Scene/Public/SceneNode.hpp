#pragma once

#include "Geometry.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace RightEngine
{
    enum class NodeType
    {
        REGULAR,
        LIGHT
    };

    class SceneNode: public std::enable_shared_from_this<SceneNode>
    {
    public:
        SceneNode() = default;
        ~SceneNode() = default;

        glm::mat4 GetWorldModelMatrix() const;
        glm::mat4 GetLocalModelMatrix() const;

        void SetGeometry(const std::shared_ptr<Geometry>& geometry);
        const std::shared_ptr<Geometry>& GetGeometry() const;

        const glm::vec3& GetLocalPosition() const;
        void SetPosition(const glm::vec3& newPosition);

        const glm::vec3& GetWorldPosition() const;

        const glm::vec3& GetRotation() const;
        void SetRotation(const glm::vec3& newRotation);

        const glm::vec3& GetScale() const;
        void SetScale(const glm::vec3& newScale);

        virtual void Draw() const;

        virtual void OnUpdate();

        void AddChild(const std::shared_ptr<SceneNode>& node);
        void RemoveChild(const std::shared_ptr<SceneNode>& node);

        const std::vector<std::shared_ptr<SceneNode>>& GetChildren() const;
        std::vector<std::shared_ptr<SceneNode>> GetAllChildren() const;

        NodeType GetBaseType() const;

    protected:
        glm::vec3 position{ 0.0f, 0.0f, 0.0f };
        glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        std::shared_ptr<Geometry> geometry;

        std::weak_ptr<SceneNode> parent;
        std::vector<std::shared_ptr<SceneNode>> children;

        NodeType baseType{ NodeType::REGULAR };

    private:
        void GetAllChildren(std::vector<std::shared_ptr<SceneNode>>& allChildren) const;
        void RecalculateTransform();
    };
}