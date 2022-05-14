#include "SandboxLayer.hpp"
#include "Renderer.hpp"
#include "EditorCamera.hpp"
#include "RendererCommand.hpp"
#include "UniformBuffer.hpp"
#include "Entity.hpp"
#include "Components.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <imgui.h>

enum class GeometryType
{
    CUBE,
    PLANE
};

using namespace RightEngine;

struct LayerSceneData
{
    std::shared_ptr<UniformBuffer> materialUniformBuffer;
    std::shared_ptr<Texture> albedoTexture;
    std::shared_ptr<Texture> normalTexture;
    std::shared_ptr<Texture> metallicTexture;
    std::shared_ptr<Texture> roughnessTexture;
    std::shared_ptr<Texture> aoTexture;
    std::shared_ptr<EditorCamera> camera;
};

static LayerSceneData sceneData;

std::shared_ptr<RightEngine::Entity> CreateTestSceneNode(const std::shared_ptr<Scene>& scene, GeometryType type)
{
    auto node = scene->CreateEntity();
    Mesh* mesh = nullptr;
    switch (type)
    {
        case GeometryType::CUBE:
            mesh = RightEngine::MeshBuilder::CubeGeometry();
            break;
        case GeometryType::PLANE:
            mesh = RightEngine::MeshBuilder::PlaneGeometry();
            break;
    }
    node->AddComponent<Mesh>(std::move(*mesh));
    auto& textureData = node->GetComponent<Mesh>().GetMaterial()->GetTextureData();
    textureData.normal = sceneData.normalTexture;
    textureData.ao = sceneData.aoTexture;
    textureData.albedo = sceneData.albedoTexture;
    textureData.metallic = sceneData.metallicTexture;
    textureData.roughness = sceneData.roughnessTexture;
    return node;
}

void SandboxLayer::OnAttach()
{
    sceneData.albedoTexture = std::make_shared<Texture>("/Assets/Textures/albedo.png");
    sceneData.normalTexture = std::make_shared<Texture>("/Assets/Textures/normal.png");
    sceneData.roughnessTexture = std::make_shared<Texture>("/Assets/Textures/roughness.png");
    sceneData.metallicTexture = std::make_shared<Texture>("/Assets/Textures/metallic.png");
    sceneData.aoTexture = std::make_shared<Texture>("/Assets/Textures/ao.png");

    sceneData.camera = std::make_shared<RightEngine::EditorCamera>(glm::vec3(0, 5, -15),
                                                                    glm::vec3(0, 1, 0));
    scene = Scene::Create();

    const auto cube1 = CreateTestSceneNode(scene, GeometryType::CUBE);
    cube1->GetComponent<Transform>().SetPosition({0, 0.0f, 0});
    const auto cube2 = CreateTestSceneNode(scene, GeometryType::CUBE);
    cube2->GetComponent<Transform>().SetPosition({5.0f, 2.0f, 0});
    scene->SetCamera(sceneData.camera);
    scene->GetRootNode()->AddChild(cube1);
    scene->GetRootNode()->AddChild(cube2);
    shader = Shader::Create(GPU_API::OpenGL, "/Assets/Shaders/Basic/pbr.vert",
                            "/Assets/Shaders/Basic/pbr.frag");
    renderer = std::make_shared<RightEngine::Renderer>();

    RightEngine::FramebufferSpecification fbSpec;
    fbSpec.width = 1280;
    fbSpec.height = 720;
    fbSpec.attachments = RightEngine::FramebufferAttachmentSpecification(
            {
                    RightEngine::FramebufferTextureSpecification(RightEngine::FramebufferTextureFormat::RGBA8),
                    RightEngine::FramebufferTextureSpecification(RightEngine::FramebufferTextureFormat::RGBA8),
                    RightEngine::FramebufferTextureSpecification(RightEngine::FramebufferTextureFormat::Depth),
            }
    );

    frameBuffer = std::make_shared<RightEngine::Framebuffer>(fbSpec);

    sceneData.materialUniformBuffer = UniformBuffer::Create(GPU_API::OpenGL, sizeof(MaterialData), 0);
}

void SandboxLayer::OnUpdate(float ts)
{
    scene->OnUpdate();

    frameBuffer->Bind();
    RightEngine::RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer->Configure();
    renderer->BeginScene(scene);

    for (const auto& entity: scene->GetRegistry().view<Mesh>())
    {
        const auto& mesh = scene->GetRegistry().get<Mesh>(entity);
        auto data = mesh.GetMaterial()->GetMaterialData();
        const auto& textureData = mesh.GetMaterial()->GetTextureData();
        // TODO: Generate albedo and roughness flat textures for sliders value
        if (textureData.albedo)
        {
            data.albedo.x = -1.0f;
            textureData.albedo->Bind();
        }
        if (textureData.metallic)
        {
            data.metallic = -1.0;
            textureData.metallic->Bind(2);
        }
        if (textureData.roughness)
        {
            data.roughness = -1.0;
            textureData.roughness->Bind(3);
        }
        textureData.normal->Bind(1);
        textureData.ao->Bind(4);
        shader->Bind();
        const auto& transform = scene->GetRegistry().get<Transform>(entity);
        shader->SetUniform1iv("u_Textures", { 0, 1, 2, 3, 4 });
        shader->SetUniform3f("camPos", sceneData.camera->GetPosition());

        sceneData.materialUniformBuffer->SetData(&data, sizeof(MaterialData));
        renderer->SubmitMesh(shader, mesh, transform.GetWorldTransformMatrix());
    }

    renderer->EndScene();
    frameBuffer->UnBind();
}

void SandboxLayer::OnImGuiRender()
{
    // TODO: viewport scaling
    ImGui::Begin("Scene view");
    id = frameBuffer->GetColorAttachment();
    ImGui::Image((void*) id, ImVec2(1280, 720));
    ImGui::End();
}
