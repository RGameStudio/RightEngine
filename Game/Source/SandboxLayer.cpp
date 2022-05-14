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

enum class TextureSlot
{
    ALBEDO_TEXTURE_SLOT = 0,
    NORMAL_TEXTURE_SLOT,
    METALLIC_TEXTURE_SLOT,
    ROUGHNESS_TEXTURE_SLOT,
    AO_TEXTURE_SLOT
};

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

static void TryTextureBind(MaterialData& materialData, const std::shared_ptr<Texture>& texture, TextureSlot slot)
{
    bool hasTexture = false;
    if (texture)
    {
        texture->Bind(static_cast<uint32_t>(slot));
        hasTexture = true;
    }

    switch (slot)
    {
        case TextureSlot::ALBEDO_TEXTURE_SLOT:
            materialData.hasAlbedo = hasTexture;
            break;
        case TextureSlot::NORMAL_TEXTURE_SLOT:
            materialData.hasNormal = hasTexture;
            break;
        case TextureSlot::METALLIC_TEXTURE_SLOT:
            materialData.hasMetallic = hasTexture;
            break;
        case TextureSlot::ROUGHNESS_TEXTURE_SLOT:
            materialData.hasRoughness = hasTexture;
            break;
        case TextureSlot::AO_TEXTURE_SLOT:
            materialData.hasAO = hasTexture;
            break;
        default:
            R_ASSERT(false, "Unknown texture slot!");
    }
}

static void BindTextures(MaterialData& material, const TextureData& textures)
{
    TryTextureBind(material, textures.albedo, TextureSlot::ALBEDO_TEXTURE_SLOT);
    TryTextureBind(material, textures.normal, TextureSlot::NORMAL_TEXTURE_SLOT);
    TryTextureBind(material, textures.metallic, TextureSlot::METALLIC_TEXTURE_SLOT);
    TryTextureBind(material, textures.roughness, TextureSlot::ROUGHNESS_TEXTURE_SLOT);
    TryTextureBind(material, textures.ao, TextureSlot::AO_TEXTURE_SLOT);
}


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
    auto& textureData = node->GetComponent<Mesh>().GetMaterial()->textureData;
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
    cube1->GetComponent<Transform>().SetPosition({ 0, 0.0f, 0 });
    const auto cube2 = CreateTestSceneNode(scene, GeometryType::CUBE);
    cube2->GetComponent<Transform>().SetPosition({ 5.0f, 2.0f, 0 });
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
        const auto& transform = scene->GetRegistry().get<Transform>(entity);
        const auto& mesh = scene->GetRegistry().get<Mesh>(entity);

        auto& materialData = mesh.GetMaterial()->materialData;
        const auto& textureData = mesh.GetMaterial()->textureData;
        BindTextures(materialData, textureData);

        shader->Bind();
        shader->SetUniform1iv("u_Textures", { 0, 1, 2, 3, 4 });
        shader->SetUniform3f("camPos", sceneData.camera->GetPosition());

        sceneData.materialUniformBuffer->SetData(&materialData, sizeof(MaterialData));
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
