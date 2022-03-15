#include "EntryPoint.hpp"
#include "Core.h"

enum class GeometryType
{
    CUBE,
    PLANE
};

std::shared_ptr<RightEngine::SceneNode> CreateTestSceneNode(GeometryType type)
{
    const auto texture = std::make_shared<RightEngine::Texture>("/Assets/Textures/MossyWoodAlbedo.png");
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
    node->GetGeometry()->SetMaterial(std::make_shared<RightEngine::Material>());
    node->GetGeometry()->GetMaterial()->SetBaseTexture(texture);
    return node;
}

void GameApplication::OnStart()
{
    const auto camera = std::make_shared<RightEngine::FPSCamera>(glm::vec3(0, 5, -15),
                                                                 glm::vec3(0, 1, 0));
    const auto shader = std::make_shared<RightEngine::Shader>("/Assets/Shaders/Basic/basic.vert",
                                                              "/Assets/Shaders/Basic/basic.frag");
    const auto scene = std::make_shared<RightEngine::Scene>();
    const auto plane = CreateTestSceneNode(GeometryType::PLANE);
    plane->SetScale(glm::vec3(5, 1, 5));
    const auto cube = CreateTestSceneNode(GeometryType::CUBE);
    plane->AddChild(cube);
    auto& renderer = RightEngine::Renderer::Get();
    renderer.SetShader(shader);
    scene->SetCamera(camera);
    scene->GetRootNode()->AddChild(plane);
    RightEngine::Application::Get().SetScene(scene);
    RightEngine::Renderer::Get().HasDepthTest(true);
}

void GameApplication::OnUpdate()
{
}

void GameApplication::OnDestroy()
{
}
