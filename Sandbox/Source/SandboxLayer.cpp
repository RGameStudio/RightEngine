#include "SandboxLayer.hpp"
#include "RendererCommand.hpp"
#include "TextureLoader.hpp"
#include "RendererState.hpp"
#include "RendererCommand.hpp"
#include "KeyCodes.hpp"
#include "MeshLoader.hpp"
#include <imgui.h>
#include <glm/gtx/transform.hpp>

using namespace RightEngine;

namespace
{
    struct TransformConstant
    {
        glm::mat4 transform;
    };

    struct SceneUBO
    {
        glm::mat4 view;
        glm::mat4 projection;
    };

    Renderer* renderer = nullptr;

    const float vertices[] =
            {
                    -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
                    0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
                    0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
                    -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
            };

    const uint32_t indices[] = {
            0, 1, 2,
            2, 3, 0
    };

    const float cubeVertexData[] = {
            // [position 3] [normal 3] [texture coodinate 2]
            // back face
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            // front face
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            // left face
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-right
            -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
            -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-right
            // right face
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            // bottom face
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            // top face
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };

    std::shared_ptr<Buffer> vertexBuffer;
    std::shared_ptr<Buffer> indexBuffer;
    std::shared_ptr<Buffer> transformUBO;
    std::shared_ptr<Buffer> sceneUBO;
    std::shared_ptr<GraphicsPipeline> graphicsPipeline;
    std::shared_ptr<GraphicsPipeline> presentPipeline;
    std::shared_ptr<GraphicsPipeline> uiPipeline;
    TextureLoader textureLoader;
    std::shared_ptr<Texture> testTexture;
    std::shared_ptr<RendererState> rendererState;
    std::shared_ptr<ImGuiLayer> imGuiLayer;

    Camera camera(glm::vec3(0, 0, -10), glm::vec3(0, 1, 0));
    MeshLoader meshLoader;
    std::shared_ptr<MeshNode> mesh;

    void DrawMeshNode(Renderer* renderer, const std::shared_ptr<MeshNode>& node, bool recursive)
    {
        for (const auto& meshNode : mesh->meshes)
        {
            renderer->Draw(meshNode->GetVertexBuffer(), meshNode->GetIndexBuffer());
        }

        if (recursive)
        {
            for (const auto& child : node->children)
            {
                DrawMeshNode(renderer, child, true);
            }
        }
    }
}

void SandboxLayer::OnAttach()
{
    renderer = new Renderer();
    BufferDescriptor transformConstantDesc{};
    transformConstantDesc.type = BufferType::UNIFORM;
    transformConstantDesc.size = sizeof(TransformConstant);
    transformConstantDesc.memoryType = MemoryType::CPU_GPU;
    transformUBO = Device::Get()->CreateBuffer(transformConstantDesc, nullptr);

    BufferDescriptor sceneUBODesc{};
    sceneUBODesc.type = BufferType::UNIFORM;
    sceneUBODesc.size = sizeof(SceneUBO);
    sceneUBODesc.memoryType = MemoryType::CPU_GPU;
    sceneUBO = Device::Get()->CreateBuffer(sceneUBODesc, nullptr);

    ShaderProgramDescriptor shaderProgramDescriptor;
    ShaderDescriptor vertexShader;
    vertexShader.path = "/Assets/Shaders/simple.vert";
    vertexShader.type = ShaderType::VERTEX;
    ShaderDescriptor fragmentShader;
    fragmentShader.path = "/Assets/Shaders/simple.frag";
    fragmentShader.type = ShaderType::FRAGMENT;
    shaderProgramDescriptor.shaders = {vertexShader, fragmentShader};
    VertexBufferLayout layout;
    layout.Push<glm::vec3>();
    layout.Push<glm::vec3>();
    layout.Push<glm::vec2>();
    layout.Push<glm::vec3>();
    layout.Push<glm::vec3>();
    shaderProgramDescriptor.layout = layout;
    shaderProgramDescriptor.reflection.textures = { 2 };
    shaderProgramDescriptor.reflection.buffers[{ 0, ShaderType::VERTEX }] = BufferType::UNIFORM;
    shaderProgramDescriptor.reflection.buffers[{ 1, ShaderType::VERTEX }] = BufferType::UNIFORM;
    const auto shader = Device::Get()->CreateShader(shaderProgramDescriptor);

    const auto window = Application::Get().GetWindow();
    glm::ivec2 extent;
    glfwGetFramebufferSize(static_cast<GLFWwindow*>(window->GetNativeHandle()), &extent.x, &extent.y);
    GraphicsPipelineDescriptor pipelineDescriptor;
    pipelineDescriptor.shader = shader;

    camera.SetAspectRatio(window->GetWidth() / window->GetHeight());

    // Default offscreen rendering pipeline

    TextureDescriptor colorAttachmentDesc{};
    colorAttachmentDesc.format = Format::BGRA8_SRGB;
    colorAttachmentDesc.type = TextureType::TEXTURE_2D;
    colorAttachmentDesc.width = extent.x;
    colorAttachmentDesc.height = extent.y;
    colorAttachmentDesc.componentAmount = 4;
    const auto colorAttachment = Device::Get()->CreateTexture(colorAttachmentDesc, {});
    TextureDescriptor depthAttachmentDesc{};
    depthAttachmentDesc.format = Format::D32_SFLOAT_S8_UINT;
    depthAttachmentDesc.type = TextureType::TEXTURE_2D;
    depthAttachmentDesc.width = extent.x;
    depthAttachmentDesc.height = extent.y;
    const auto depthAttachment =  Device::Get()->CreateTexture(depthAttachmentDesc, {});

    RenderPassDescriptor renderPassDescriptor{};
    renderPassDescriptor.extent = extent;
    renderPassDescriptor.offscreen = true;
    AttachmentDescriptor depth{};
    depth.loadOperation = AttachmentLoadOperation::CLEAR;
    depth.texture = depthAttachment;
    AttachmentDescriptor color{};
    color.texture = colorAttachment;
    color.loadOperation = AttachmentLoadOperation::CLEAR;
    renderPassDescriptor.colorAttachments = { color };
    renderPassDescriptor.depthStencilAttachment = { depth };

    graphicsPipeline = Device::Get()->CreateGraphicsPipeline(pipelineDescriptor, renderPassDescriptor);
    renderer->SetPipeline(graphicsPipeline);

    // Pipeline for presenting on screen
    GraphicsPipelineDescriptor presentPipelineDesc{};
    presentPipelineDesc.shader = shader;
    RenderPassDescriptor presentRenderPassDescriptor{};
    presentRenderPassDescriptor.extent = extent;
    presentRenderPassDescriptor.offscreen = false;
    AttachmentDescriptor presentColor{};
    presentColor.texture = colorAttachment;
    presentColor.loadOperation = AttachmentLoadOperation::LOAD;
    presentRenderPassDescriptor.colorAttachments = { presentColor };
    presentRenderPassDescriptor.depthStencilAttachment = { depth };

    presentPipeline = Device::Get()->CreateGraphicsPipeline(presentPipelineDesc,
                                                            presentRenderPassDescriptor);

    // Textures loading
    auto [data, descriptor] = textureLoader.Load("/Assets/Textures/diffuse.jpg", {Format::NONE, true, true});
    descriptor.type = TextureType::TEXTURE_2D;
    testTexture = Device::Get()->CreateTexture(descriptor, data);
    SamplerDescriptor samplerDescriptor{};
    testTexture->SetSampler(Device::Get()->CreateSampler(samplerDescriptor));

    rendererState = RendererCommand::CreateRendererState();
    rendererState->SetVertexBuffer(sceneUBO, 1);
    rendererState->SetVertexBuffer(transformUBO, 0);
    rendererState->SetTexture(testTexture, 2);
    rendererState->OnUpdate(graphicsPipeline);

    EventDispatcher::Get().Subscribe(MouseMovedEvent::descriptor, EVENT_CALLBACK(SandboxLayer::OnEvent));
    camera.SetMovementSpeed(20.0f);
    camera.SetActive(false);

    mesh = meshLoader.Load("/Assets/Models/backpack.obj");

    GraphicsPipelineDescriptor uiPipelineDesc{};
    uiPipelineDesc.shader = nullptr;

    AttachmentDescriptor uiAttachment{};
    uiAttachment.texture = colorAttachment;
    uiAttachment.loadOperation = AttachmentLoadOperation::LOAD;
    uiAttachment.storeOperation = AttachmentStoreOperation::STORE;

    RenderPassDescriptor uiRenderpass{};
    uiRenderpass.extent = extent;
    uiRenderpass.offscreen = true;
    uiRenderpass.colorAttachments.emplace_back(uiAttachment);

    uiPipeline = Device::Get()->CreateGraphicsPipeline(uiPipelineDesc, uiRenderpass);

    imGuiLayer = std::make_shared<ImGuiLayer>(uiPipeline);
    Application::Get().PushOverlay(imGuiLayer);
}

void SandboxLayer::OnUpdate(float ts)
{
    if (Input::IsKeyDown(R_KEY_W))
    {
        camera.Move(R_KEY_W);
    }
    if (Input::IsKeyDown(R_KEY_S))
    {
        camera.Move(R_KEY_S);
    }
    if (Input::IsKeyDown(R_KEY_A))
    {
        camera.Move(R_KEY_A);
    }
    if (Input::IsKeyDown(R_KEY_D))
    {
        camera.Move(R_KEY_D);
    }

    camera.OnUpdate(ts);

    TransformConstant transformConstantValue;
    SceneUBO sceneUboValue;
    transformConstantValue.transform = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime() * glm::radians(90.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
    sceneUboValue.view = camera.GetViewMatrix();
    sceneUboValue.projection = camera.GetProjectionMatrix();
    sceneUboValue.projection[1][1] *= -1;
    auto transformConstantValuePtr = transformUBO->Map();
    memcpy(transformConstantValuePtr, &transformConstantValue, sizeof(TransformConstant));
    transformUBO->UnMap();
    transformUBO->SetNeedToSync(true);
    auto sceneUboPtr = sceneUBO->Map();
    memcpy(sceneUboPtr, &sceneUboValue, sizeof(SceneUBO));
    sceneUBO->UnMap();
    sceneUBO->SetNeedToSync(true);

    renderer->SetPipeline(graphicsPipeline);
    renderer->BeginFrame(nullptr);
    rendererState->OnUpdate(graphicsPipeline);
    renderer->EncodeState(rendererState);
    DrawMeshNode(renderer, mesh, true);
    renderer->EndFrame();

    renderer->SetPipeline(uiPipeline);
    renderer->BeginFrame(nullptr);
    imGuiLayer->Begin();
    ImGui::Begin("FPS");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
    imGuiLayer->End(renderer->GetCmd());
    renderer->EndFrame();

    renderer->SetPipeline(presentPipeline);
    renderer->BeginFrame(nullptr);
    renderer->EndFrame();
}

bool SandboxLayer::OnEvent(const Event& event)
{
    if (event.GetType() == MouseMovedEvent::descriptor)
    {
        MouseMovedEvent mouseMovedEvent = static_cast<const MouseMovedEvent&>(event);
        camera.Rotate(mouseMovedEvent.GetX(), mouseMovedEvent.GetY());
        return true;
    }

    return false;
}
