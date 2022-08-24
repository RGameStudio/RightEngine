#include "SandboxLayer.hpp"
#include "RendererCommand.hpp"
#include "TextureLoader.hpp"
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
    std::shared_ptr<Buffer> transformConstant;
    std::shared_ptr<Buffer> sceneUBO;
    std::shared_ptr<GraphicsPipeline> graphicsPipeline;
    TextureLoader textureLoader;
    std::shared_ptr<Texture> testTexture;
}

void SandboxLayer::OnAttach()
{
    renderer = new Renderer();
    std::cout << "Hello!" << std::endl;
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
    transformConstantDesc.type = BUFFER_TYPE_CONSTANT;
    transformConstantDesc.size = sizeof(TransformConstant);
    transformConstant = Device::Get()->CreateBuffer(transformConstantDesc, nullptr);

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
    const auto shader = Device::Get()->CreateShader(shaderProgramDescriptor);

    const auto window = Application::Get().GetWindow();
    glm::ivec2 extent;
    glfwGetFramebufferSize(static_cast<GLFWwindow*>(window->GetNativeHandle()), &extent.x, &extent.y);
    GraphicsPipelineDescriptor pipelineDescriptor;
    pipelineDescriptor.shader = shader;
    pipelineDescriptor.extent = extent;
    pipelineDescriptor.vertexBuffers[-1] = transformConstant;
    pipelineDescriptor.vertexBuffers[0] = sceneUBO;
    RenderPassDescriptor renderPassDescriptor;
    renderPassDescriptor.format = Format::BGRA8_SRGB;

    graphicsPipeline = Device::Get()->CreateGraphicsPipeline(pipelineDescriptor, renderPassDescriptor);
    renderer->SetPipeline(graphicsPipeline);

    auto [data, texDesc] = textureLoader.Load("/Assets/Textures/albedo.png");
    texDesc.type = TextureType::TEXTURE_2D;
    testTexture = Device::Get()->CreateTexture(texDesc, data);
}

void SandboxLayer::OnUpdate(float ts)
{
    renderer->BeginFrame(nullptr);
    TransformConstant transformConstantValue;
    SceneUBO sceneUboValue;
    transformConstantValue.transform = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime() * glm::radians(90.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
    sceneUboValue.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    sceneUboValue.projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080, 0.1f, 10.0f);
    sceneUboValue.projection[1][1] *= -1;
    auto transformConstantValuePtr = transformConstant->Map();
    memcpy(transformConstantValuePtr, &transformConstantValue, sizeof(TransformConstant));
    transformConstant->UnMap();
    auto sceneUboPtr = sceneUBO->Map();
    memcpy(sceneUboPtr, &sceneUboValue, sizeof(SceneUBO));
    sceneUBO->UnMap();
    renderer->UpdateBuffer(transformConstant, ShaderType::VERTEX);
    renderer->UpdateBuffer(sceneUBO, ShaderType::VERTEX);
    renderer->Draw(vertexBuffer);
    renderer->EndFrame();
}

void SandboxLayer::OnImGuiRender()
{

}
