#include "EntryPoint.hpp"
#include "Core.h"
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

void GameApplication::OnStart()
{
    const auto camera = std::make_shared<RightEngine::FPSCamera>(glm::vec3(0, 5, -15),
                                                                 glm::vec3(0, 1, 0));
    const auto shader = std::make_shared<RightEngine::Shader>("/Assets/Shaders/Basic/basic.vert",
                                                              "/Assets/Shaders/Basic/basic.frag");
    const auto scene = std::make_shared<RightEngine::Scene>();
    const auto plane = CreateTestSceneNode(GeometryType::PLANE, "/Assets/Textures/MossyWoodAlbedo.png");
    plane->SetScale(glm::vec3(10, 1, 10));
    const auto cube = CreateTestSceneNode(GeometryType::CUBE, "");
    plane->AddChild(cube);
    cube->SetPosition({ 0, 1.0f, 0 });

    const auto pointLight = std::make_shared<RightEngine::LightNode>(RightEngine::LightNodeType::POINT_LIGHT);
    pointLight->SetPosition({ 0.0f, 5.0f, 0.0f});
    pointLight->SetColor({ 1.0f, 0.2f, 0.0f });
    cube->AddChild(pointLight);
    const auto light = std::make_shared<RightEngine::LightNode>(RightEngine::LightNodeType::AMBIENT);
    light->SetIntensity(0.1f);
    light->SetColor({ 1.0f, 1.0f, 1.0f });

    auto& renderer = RightEngine::Renderer::Get();
    renderer.SetShader(shader);
    scene->SetCamera(camera);
    scene->GetRootNode()->AddChild(plane);
    scene->GetRootNode()->AddChild(light);
    RightEngine::Application::Get().SetScene(scene);
    RightEngine::Renderer::Get().HasDepthTest(true);
}

void GameApplication::OnUpdate()
{
}

void GameApplication::OnDestroy()
{
}
