#include "Components.hpp"

using namespace RightEngine;

MeshComponent::MeshComponent()
{
    material = std::make_shared<Material>();
}

const std::shared_ptr<VertexArray>& MeshComponent::GetVertexArray() const
{
    return vertexArray;
}

const std::shared_ptr<Material>& MeshComponent::GetMaterial() const
{
    return material;
}

void MeshComponent::SetMaterial(const std::shared_ptr<Material>& newMaterial)
{
    material = newMaterial;
}

void MeshComponent::SetVertexArray(std::shared_ptr<VertexArray>& newVertexArray)
{
    vertexArray = newVertexArray;
}
