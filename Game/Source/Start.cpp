#include "EntryPoint.hpp"
#include "Core.h"

static const float cubeVertexData[] = {
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
};

std::shared_ptr<RightEngine::SceneNode> CreateTestSceneNode()
{
    RightEngine::VertexBufferLayout layout;
    layout.Push<float>(3);
    const auto geometry = std::make_shared<RightEngine::Geometry>();
    geometry->CreateVertexBuffer(cubeVertexData, sizeof(cubeVertexData));
    geometry->CreateVertexArray(layout);
    const auto node = std::make_shared<RightEngine::SceneNode>();
    node->SetGeometry(geometry);

    return node;
}

void GameApplication::OnStart()
{
    const auto camera = std::make_shared<RightEngine::FPSCamera>(glm::vec3(0, 5, -15),
                                                                 glm::vec3(0, 1, 0));
    const auto shader = std::make_shared<RightEngine::Shader>("/Assets/Shaders/Basic/basic.vert",
                                                              "/Assets/Shaders/Basic/basic.frag");
    const auto scene = std::make_shared<RightEngine::Scene>();
    const auto sceneNode = CreateTestSceneNode();
    auto& renderer = RightEngine::Renderer::Get();
    renderer.SetShader(shader);
    scene->SetCamera(camera);
    scene->GetRootNode()->AddChild(sceneNode);
    RightEngine::Application::Get().SetScene(scene);
}

void GameApplication::OnUpdate()
{
}

void GameApplication::OnDestroy()
{
}
