#include "SandboxLayer.hpp"
#include "Renderer.hpp"
#include "LightNode.hpp"

enum class GeometryType
{
    CUBE,
    PLANE
};

std::shared_ptr<RightEngine::SceneNode> CreateTestSceneNode(GeometryType type, const std::string& texturePath)
{
    std::shared_ptr<RightEngine::Texture> texture;
    if (!texturePath.empty())
    {
        texture = std::make_shared<RightEngine::Texture>(texturePath);
    }
    auto node = std::make_shared<RightEngine::SceneNode>();
    std::shared_ptr<RightEngine::Geometry> geometry;
    switch (type)
    {
        case GeometryType::CUBE:
            geometry = RightEngine::GeometryBuilder::CubeGeometry();
            break;
        case GeometryType::PLANE:
            geometry = RightEngine::GeometryBuilder::PlaneGeometry();
            break;
    }
    node->SetGeometry(geometry);
    node->GetGeometry()->GetMaterial()->SetBaseTexture(texture);
    return node;
}

void SandboxLayer::OnAttach()
{
    const auto camera = std::make_shared<RightEngine::FPSCamera>(glm::vec3(0, 5, -15),
                                                                 glm::vec3(0, 1, 0));
    scene = std::make_shared<RightEngine::Scene>();

    const auto cube1 = CreateTestSceneNode(GeometryType::CUBE, "/Assets/Textures/WoodAlbedo.png");
    cube1->SetPosition({ 0, 0.0f, 0 });
    const auto cube2 = CreateTestSceneNode(GeometryType::CUBE, "");
    cube2->SetPosition({ 5.0f, 2.0f, 0 });
    scene->SetCamera(camera);
    scene->GetRootNode()->AddChild(cube1);
    scene->GetRootNode()->AddChild(cube2);
    shader = std::make_shared<RightEngine::Shader>("/Assets/Shaders/Basic/basic.vert", "/Assets/Shaders/Basic/basic.frag");
    RightEngine::Renderer::Get().HasDepthTest(true);
}

void SandboxLayer::OnUpdate(float ts)
{
    scene->OnUpdate();
    RightEngine::Renderer::Get().BeginScene(scene);
    scene->OnRender(shader);
    RightEngine::Renderer::Get().EndScene();
}
