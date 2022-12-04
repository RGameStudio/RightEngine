#include "Components.hpp"
#include "MeshBuilder.hpp"

using namespace RightEngine;

MeshComponent::MeshComponent()
{
    material = std::make_shared<Material>();
    // TODO: Remove that temporary cube mesh creation in favor of proper asset management with content panel
    mesh = MeshBuilder::Cube();
}