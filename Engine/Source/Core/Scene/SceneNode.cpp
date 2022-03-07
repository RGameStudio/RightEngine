#include "SceneNode.hpp"
#include "Renderer.hpp"
#include <glm/ext/matrix_transform.hpp>

using namespace RightEngine;

void RightEngine::SceneNode::Draw() const
{
    if (geometry)
    {
        Renderer::Get().Draw(geometry);
    }
}

glm::mat4 RightEngine::SceneNode::GetModelMatrix() const
{
    glm::mat4 model(1);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);
    return model;
}

const std::shared_ptr<Geometry>& RightEngine::SceneNode::GetGeometry() const
{
    return geometry;
}

void RightEngine::SceneNode::SetGeometry(const std::shared_ptr<Geometry>& geometry)
{
    this->geometry = geometry;
}

const glm::vec3& SceneNode::GetPosition() const
{
    return position;
}

void SceneNode::SetPosition(const glm::vec3& position)
{
    this->position = position;
}

void SceneNode::OnUpdate()
{}

void SceneNode::AddChild(const std::shared_ptr<SceneNode>& node)
{
    children.push_back(node);
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
