#include "LightNode.hpp"

using namespace RightEngine;

RightEngine::LightNode::LightNode(RightEngine::LightNodeType type): SceneNode(), type(type)
{
    baseType = NodeType::LIGHT;
}

const glm::vec3& LightNode::GetColor() const
{
    return color;
}

void LightNode::SetColor(const glm::vec3& newColor)
{
    color = newColor;
}

LightNodeType LightNode::GetType() const
{
    return type;
}

float LightNode::GetIntensity() const
{
    return intensity;
}

void LightNode::SetIntensity(float newIntensity)
{
    intensity = newIntensity;
}
