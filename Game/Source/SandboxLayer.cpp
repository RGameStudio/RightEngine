#include "SandboxLayer.hpp"
#include "Renderer.hpp"
#include "EditorCamera.hpp"
#include "RendererCommand.hpp"
#include "UniformBuffer.hpp"
#include "Entity.hpp"
#include "Components.hpp"
#include "Texture3D.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <imgui.h>

float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
};

enum class TextureSlot
{
    ALBEDO_TEXTURE_SLOT = 0,
    NORMAL_TEXTURE_SLOT,
    METALLIC_TEXTURE_SLOT,
    ROUGHNESS_TEXTURE_SLOT,
    AO_TEXTURE_SLOT,
    SKYBOX_TEXTURE_SLOT
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
    std::shared_ptr<Entity> hdrCube;
    std::shared_ptr<Shader> hdrShader;
    std::shared_ptr<Texture> hdrTexture;
    std::shared_ptr<Entity> skyboxCube;
    std::shared_ptr<Shader> skyboxShader;
    std::shared_ptr<Texture3D> skyboxTexture;
    std::shared_ptr<Framebuffer> skyboxFramebuffer;
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

std::shared_ptr<RightEngine::Entity> CreateTestSceneNode(const std::shared_ptr<Scene>& scene,
                                                         GeometryType type)
{
    std::shared_ptr<Entity> node = scene->CreateEntity();
    Mesh* mesh = nullptr;
    switch (type)
    {
        case GeometryType::CUBE:
            mesh = MeshBuilder::CubeGeometry();
            break;
        case GeometryType::PLANE:
            mesh = MeshBuilder::PlaneGeometry();
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
    sceneData.albedoTexture = Texture::Create("/Assets/Textures/albedo.png");
    sceneData.normalTexture = Texture::Create("/Assets/Textures/normal.png");
    sceneData.roughnessTexture = Texture::Create("/Assets/Textures/roughness.png");
    sceneData.metallicTexture = Texture::Create("/Assets/Textures/metallic.png");
    sceneData.aoTexture = Texture::Create("/Assets/Textures/ao.png");

    sceneData.camera = std::make_shared<EditorCamera>(glm::vec3(0, 5, -15),
                                                                   glm::vec3(0, 1, 0));
    scene = Scene::Create();

    const auto cube1 = CreateTestSceneNode(scene, GeometryType::CUBE);
    cube1->GetComponent<Transform>().SetPosition({ 0, 0.0f, 0 });
    const auto cube2 = CreateTestSceneNode(scene, GeometryType::CUBE);
    cube2->GetComponent<Transform>().SetPosition({ 5.0f, 2.0f, 0 });
    scene->SetCamera(sceneData.camera);
    scene->GetRootNode()->AddChild(cube1);
    scene->GetRootNode()->AddChild(cube2);
    shader = Shader::Create("/Assets/Shaders/Basic/pbr.vert",
                            "/Assets/Shaders/Basic/pbr.frag");
    renderer = std::make_shared<Renderer>();

    FramebufferSpecification fbSpec;
    fbSpec.width = 1280;
    fbSpec.height = 720;
    fbSpec.attachments = FramebufferAttachmentSpecification(
            {
                    FramebufferTextureSpecification(FramebufferTextureFormat::RGBA8),
                    FramebufferTextureSpecification(FramebufferTextureFormat::RGBA8),
                    FramebufferTextureSpecification(FramebufferTextureFormat::Depth),
            }
    );

    frameBuffer = std::make_shared<Framebuffer>(fbSpec);

    sceneData.materialUniformBuffer = UniformBuffer::Create(GPU_API::OpenGL, sizeof(MaterialData), 0);

    sceneData.hdrShader = Shader::Create("/Assets/Shaders/Basic/hdr_to_cubemap.vert",
                                         "/Assets/Shaders/Basic/hdr_to_cubemap.frag");
    sceneData.hdrTexture = Texture::Create("/Assets/Textures/env1.hdr");
    sceneData.hdrCube = CreateTestSceneNode(scene, GeometryType::CUBE);
    //Turn off all textures
//    sceneData.hdrCube->GetComponent<Mesh>().GetMaterial()->textureData = TextureData();
//    sceneData.hdrCube->GetComponent<Transform>().SetPosition({ 0.0f, -2.0f, 5.0f });
//    scene->GetRootNode()->AddChild(sceneData.hdrCube);

    sceneData.skyboxShader = Shader::Create("/Assets/Shaders/Basic/skybox.vert",
                                            "/Assets/Shaders/Basic/skybox.frag");
    sceneData.skyboxTexture = Texture3D::Create(
    {
        "/Assets/Textures/cube_c00.hdr",
        "/Assets/Textures/cube_c01.hdr",
        "/Assets/Textures/cube_c02.hdr",
        "/Assets/Textures/cube_c03.hdr",
        "/Assets/Textures/cube_c04.hdr",
        "/Assets/Textures/cube_c05.hdr",
    });
    sceneData.skyboxCube = scene->CreateEntity();
    VertexBufferLayout layout;
    layout.Push<float>(3);
    Mesh mesh;
    auto vertexArray = std::make_shared<VertexArray>();
    vertexArray->AddBuffer(std::make_shared<VertexBuffer>(skyboxVertices, sizeof(skyboxVertices)), layout);
    mesh.SetVertexArray(vertexArray);
    mesh.SetVisibility(false);
    sceneData.skyboxCube->AddComponent<Mesh>(mesh);

    sceneData.skyboxCube->GetComponent<Transform>().SetPosition({ 0.0f, -0.0f, 0.0f });
    scene->GetRootNode()->AddChild(sceneData.skyboxCube);
}

void SandboxLayer::OnUpdate(float ts)
{
    scene->OnUpdate();

    frameBuffer->Bind();
    RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer->Configure();
    renderer->BeginScene(scene);
    for (const auto& entity: scene->GetRegistry().view<Mesh>())
    {
        const auto& transform = scene->GetRegistry().get<Transform>(entity);
        const auto& mesh = scene->GetRegistry().get<Mesh>(entity);
        if (!mesh.IsVisible())
        {
            continue;
        }

        auto& materialData = mesh.GetMaterial()->materialData;
        const auto& textureData = mesh.GetMaterial()->textureData;
        BindTextures(materialData, textureData);

        shader->Bind();
        shader->SetUniform1iv("u_Textures", { 0, 1, 2, 3, 4 });
        shader->SetUniform3f("camPos", sceneData.camera->GetPosition());

        sceneData.materialUniformBuffer->SetData(&materialData, sizeof(MaterialData));
        renderer->SubmitMesh(shader, mesh, transform.GetWorldTransformMatrix());
    }
    shader->UnBind();
    frameBuffer->UnBind();

    FramebufferSpecification fbSpec;
    fbSpec.width = 1280;
    fbSpec.height = 720;
    fbSpec.attachments = FramebufferAttachmentSpecification(
            {
                    FramebufferTextureSpecification(FramebufferTextureFormat::RGBA8),
                    FramebufferTextureSpecification(FramebufferTextureFormat::RGBA8),
                    FramebufferTextureSpecification(FramebufferTextureFormat::Depth),
            }
    );

    sceneData.skyboxFramebuffer = std::make_shared<Framebuffer>(fbSpec);
    frameBuffer->Bind();
//    RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    sceneData.skyboxShader->Bind();
    sceneData.skyboxTexture->Bind(static_cast<uint32_t>(TextureSlot::SKYBOX_TEXTURE_SLOT));
    sceneData.skyboxShader->SetUniform1i("u_Skybox", static_cast<uint32_t>(TextureSlot::SKYBOX_TEXTURE_SLOT));
    const auto projectionMatrix = glm::perspective(glm::radians(45.0f),
                                                   16.0f/ 9.0f,
                                                   0.1f,
                                                   300.0f);
    const auto viewMatrix = glm::mat4(glm::mat3(scene->GetCamera()->GetViewMatrix()));
    sceneData.skyboxShader->SetUniformMat4f("u_Projection", projectionMatrix);
    sceneData.skyboxShader->SetUniformMat4f("u_View", viewMatrix);
    auto& rendererSettings = renderer->GetSettings();
    rendererSettings.depthTestMode = RightEngine::DepthTestMode::LEQUAL;
    renderer->Configure();
    renderer->SubmitMesh(sceneData.skyboxShader,
                         sceneData.skyboxCube->GetComponent<Mesh>(),
                         sceneData.skyboxCube->GetComponent<Transform>().GetWorldTransformMatrix());
    rendererSettings.depthTestMode = RightEngine::DepthTestMode::LESS;
    renderer->Configure();
    frameBuffer->UnBind();
    renderer->EndScene();
}

void SandboxLayer::OnImGuiRender()
{
    // TODO: viewport scaling with imgui window size change
    ImGui::Begin("Scene view");
    id = frameBuffer->GetColorAttachment();
    ImGui::Image((void*) id, ImVec2(1280, 720), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}
