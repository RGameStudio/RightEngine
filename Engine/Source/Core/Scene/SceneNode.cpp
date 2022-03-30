#include "SceneNode.hpp"
#include "Renderer.hpp"
#include <glm/ext/matrix_transform.hpp>

using namespace RightEngine;

glm::mat4 RightEngine::SceneNode::GetLocalTransformMatrix() const
{
    glm::mat4 model(1);
    const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
                                             glm::radians(rotation.x),
                                             glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
                                             glm::radians(rotation.y),
                                             glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
                                             glm::radians(rotation.z),
                                             glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

    return glm::translate(glm::mat4(1.0f), position) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);
}

glm::mat4 SceneNode::GetWorldTransformMatrix() const
{
    return modelMatrix;
}

const std::shared_ptr<Geometry>& RightEngine::SceneNode::GetGeometry() const
{
    return geometry;
}

void RightEngine::SceneNode::SetGeometry(const std::shared_ptr<Geometry>& geometry)
{
    this->geometry = geometry;
}

const glm::vec3& SceneNode::GetLocalPosition() const
{
    return position;
}

const glm::vec3& SceneNode::GetWorldPosition() const
{
    return modelMatrix[3];
}

void SceneNode::SetPosition(const glm::vec3& newPosition)
{
    position = newPosition;
}

void SceneNode::OnUpdate()
{
    RecalculateTransform();
}

void SceneNode::AddChild(const std::shared_ptr<SceneNode>& node)
{
    children.push_back(node);
    node->parent = shared_from_this();
}

void SceneNode::RemoveChild(const std::shared_ptr<SceneNode>& node)
{
    for (int index = 0; index < children.size(); index++)
    {
        if (node == children[index])
        {
            children.erase(children.begin() + index);
            break;
        }
    }
}

const std::vector<std::shared_ptr<SceneNode>>& SceneNode::GetChildren() const
{
    return children;
}

std::vector<std::shared_ptr<SceneNode>> SceneNode::GetAllChildren() const
{
    std::vector<std::shared_ptr<SceneNode>> allChildren;
    GetAllChildren(allChildren);
    return allChildren;
}

void SceneNode::GetAllChildren(std::vector<std::shared_ptr<SceneNode>>& allChildren) const
{
    for (const auto& node: children)
    {
        node->GetAllChildren(allChildren);
        allChildren.push_back(node);
    }
}

const glm::vec3& SceneNode::GetScale() const
{
    return scale;
}

void SceneNode::SetScale(const glm::vec3& newScale)
{
    scale = newScale;
}

NodeType SceneNode::GetBaseType() const
{
    return baseType;
}

void SceneNode::RecalculateTransform()
{
    // TODO: Add dirty flag
    const auto parentPtr = parent.lock();
    if (parentPtr)
    {
        modelMatrix = parentPtr->modelMatrix * GetLocalTransformMatrix();
    }
    else
    {
        modelMatrix = GetLocalTransformMatrix();
    }

    for (auto& child: children)
    {
        child->RecalculateTransform();
    }
}

const glm::vec3& SceneNode::GetRotation() const
{
    return rotation;
}

void SceneNode::SetRotation(const glm::vec3& newRotation)
{
    rotation = newRotation;
}
