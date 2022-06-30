#include "Components.hpp"

using namespace RightEngine;

Mesh::Mesh()
{
    material = std::make_shared<Material>();
}

const std::shared_ptr<VertexArray>& Mesh::GetVertexArray() const
{
    return vertexArray;
}

const std::shared_ptr<Material>& Mesh::GetMaterial() const
{
    return material;
}

void Mesh::SetMaterial(const std::shared_ptr<Material>& newMaterial)
{
    material = newMaterial;
}

void Mesh::SetVertexArray(std::shared_ptr<VertexArray>& newVertexArray)
{
    vertexArray = newVertexArray;
}
