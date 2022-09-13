#include "SandboxLayer.hpp"
#include "RendererCommand.hpp"
#include "TextureLoader.hpp"
#include "RendererState.hpp"
#include "RendererCommand.hpp"
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
    TextureLoader textureLoader;
    std::shared_ptr<Texture> testTexture;
    std::shared_ptr<RendererState> rendererState;
}

void SandboxLayer::OnAttach()
{
    renderer = new Renderer();
    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.type = BUFFER_TYPE_VERTEX;
    bufferDescriptor.size = sizeof(cubeVertexData);
    bufferDescriptor.memoryType = static_cast<MemoryType>(MEMORY_TYPE_HOST_COHERENT | MEMORY_TYPE_HOST_VISIBLE);
    vertexBuffer = Device::Get()->CreateBuffer(bufferDescriptor, cubeVertexData);

    BufferDescriptor indexBufferDescriptor{};
    indexBufferDescriptor.type = BUFFER_TYPE_INDEX;
    indexBufferDescriptor.size = sizeof(indices);
    indexBufferDescriptor.memoryType = static_cast<MemoryType>(MEMORY_TYPE_HOST_COHERENT | MEMORY_TYPE_HOST_VISIBLE);
    indexBuffer = Device::Get()->CreateBuffer(indexBufferDescriptor, indices);

    BufferDescriptor transformConstantDesc{};
    transformConstantDesc.type = BUFFER_TYPE_UNIFORM;
    transformConstantDesc.size = sizeof(TransformConstant);
    transformConstantDesc.memoryType = static_cast<MemoryType>(MEMORY_TYPE_HOST_COHERENT | MEMORY_TYPE_HOST_VISIBLE);
    transformUBO = Device::Get()->CreateBuffer(transformConstantDesc, nullptr);

    BufferDescriptor sceneUBODesc{};
    sceneUBODesc.type = BUFFER_TYPE_UNIFORM;
    sceneUBODesc.size = sizeof(SceneUBO);
    sceneUBODesc.memoryType = static_cast<MemoryType>(MEMORY_TYPE_HOST_COHERENT | MEMORY_TYPE_HOST_VISIBLE);
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
    shaderProgramDescriptor.layout = layout;
    shaderProgramDescriptor.reflection.textures = { 2 };
    shaderProgramDescriptor.reflection.buffers[{ 0, ShaderType::VERTEX }] = BUFFER_TYPE_UNIFORM;
    shaderProgramDescriptor.reflection.buffers[{ 1, ShaderType::VERTEX }] = BUFFER_TYPE_UNIFORM;
    const auto shader = Device::Get()->CreateShader(shaderProgramDescriptor);

    const auto window = Application::Get().GetWindow();
    glm::ivec2 extent;
    glfwGetFramebufferSize(static_cast<GLFWwindow*>(window->GetNativeHandle()), &extent.x, &extent.y);
    GraphicsPipelineDescriptor pipelineDescriptor;
    pipelineDescriptor.shader = shader;

    // Default offscreen rendering pipeline

    TextureDescriptor colorAttachmentDesc{};
    colorAttachmentDesc.format = Format::BGRA8_SRGB;
    colorAttachmentDesc.type = TextureType::TEXTURE_2D;
    colorAttachmentDesc.width = extent.x;
    colorAttachmentDesc.height = extent.y;
    const auto colorAttachment = Device::Get()->CreateTexture(colorAttachmentDesc, {});
    TextureDescriptor depthAttachmentDesc{};
    depthAttachmentDesc.format = Format::D24_UNORM_S8_UINT;
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

    auto [data, texDesc] = textureLoader.Load("/Assets/Textures/albedo.png");
    texDesc.type = TextureType::TEXTURE_2D;
    texDesc.format = Format::RGBA8_SRGB;
    testTexture = Device::Get()->CreateTexture(texDesc, data);
    SamplerDescriptor samplerDescriptor{};
    testTexture->SetSampler(Device::Get()->CreateSampler(samplerDescriptor));

    rendererState = RendererCommand::CreateRendererState();
    rendererState->SetVertexBuffer(sceneUBO, 1);
    rendererState->SetVertexBuffer(transformUBO, 0);
    rendererState->SetTexture(testTexture, 2);
    rendererState->OnUpdate(graphicsPipeline);

    TextureDescriptor cubeDesc;
    cubeDesc.type = TextureType::CUBEMAP;
    cubeDesc.width = 1024;
    cubeDesc.height = 1024;
    cubeDesc.format = Format::RGBA16_SFLOAT;
    cubeDesc.componentAmount = 3;
    const auto cubemap = Device::Get()->CreateTexture(cubeDesc, {});
    int kek;
}

void SandboxLayer::OnUpdate(float ts)
{
    TransformConstant transformConstantValue;
    SceneUBO sceneUboValue;
    transformConstantValue.transform = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime() * glm::radians(90.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
    sceneUboValue.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    sceneUboValue.projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080, 0.1f, 10.0f);
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
    renderer->Draw(vertexBuffer);
    renderer->EndFrame();

    renderer->SetPipeline(presentPipeline);
    renderer->BeginFrame(nullptr);
    renderer->EndFrame();
}

void SandboxLayer::OnImGuiRender()
{

}
