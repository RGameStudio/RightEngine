#include "EntryPoint.hpp"
#include "Core.h"
#include "LightNode.hpp"
#include "Shaders/BasicShader.hpp"

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
    const auto shader = std::make_shared<RightEngine::BasicShader>();
    const auto scene = std::make_shared<RightEngine::Scene>();

    const auto cube1 = CreateTestSceneNode(GeometryType::CUBE, "");
    cube1->SetPosition({ 0, 0.0f, 0 });
    const auto cube2 = CreateTestSceneNode(GeometryType::CUBE, "");
    cube2->SetPosition({ 5.0f, 2.0f, 0 });
    const auto cube3 = CreateTestSceneNode(GeometryType::CUBE, "");
    cube3->SetPosition({ 5.0f, 0.0f, 0 });

    cube1->AddChild(cube2);
    cube2->AddChild(cube3);

    const auto light = std::make_shared<RightEngine::LightNode>(RightEngine::LightNodeType::AMBIENT);
    light->SetIntensity(1.0f);

    const auto lightCube = CreateTestSceneNode(GeometryType::CUBE, "");
    const auto pointLight = std::make_shared<RightEngine::LightNode>(RightEngine::LightNodeType::POINT_LIGHT);
    pointLight->SetColor({ 1.0f, 0.0f, 0.0f });
    lightCube->SetPosition({ 2.0f, 10.0f, 0.0f });
    lightCube->AddChild(pointLight);

    auto& renderer = RightEngine::Renderer::Get();
    renderer.SetShader(shader);
    scene->SetCamera(camera);
    scene->GetRootNode()->AddChild(cube1);
    scene->GetRootNode()->AddChild(light);
    scene->GetRootNode()->AddChild(lightCube);
    RightEngine::Application::Get().SetScene(scene);
    RightEngine::Renderer::Get().HasDepthTest(true);
}

void GameApplication::OnUpdate()
{
}

void GameApplication::OnDestroy()
{
}
