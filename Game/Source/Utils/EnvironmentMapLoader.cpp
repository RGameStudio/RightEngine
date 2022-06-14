#include "EnvironmentMapLoader.hpp"
#include "TextureLoader.hpp"
#include "MeshBuilder.hpp"
#include "Shader.hpp"
#include "FrameBuffer.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "RendererCommand.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

using namespace RightEngine;

namespace
{
    const uint32_t envTexWidth = 2048;
    const uint32_t envTexHeight = 2048;
}

void EnvironmentMapLoader::Load(const std::string& path, bool flipVertically)
{
    TextureLoader textureLoader;
    auto [data, textureSpec] = textureLoader.Load(path, flipVertically);
    const auto equirectMap = Texture::Create(textureSpec, data);

    Mesh* cube = MeshBuilder::CubeGeometry();
    //TODO: Cache shader in shader library
    const auto envmapConverterShader = Shader::Create("/Assets/Shaders/Utils/envmap_to_cubemap.vert",
                                                      "/Assets/Shaders/Utils/envmap_to_cubemap.frag");

    FramebufferSpecification fbSpec;
    fbSpec.width = envTexWidth;
    fbSpec.height = envTexHeight;
    fbSpec.attachments = FramebufferAttachmentSpecification(
            {
                    FramebufferTextureSpecification(FramebufferTextureFormat::RGBA8),
                    FramebufferTextureSpecification(FramebufferTextureFormat::DEPTH24STENCIL8)
            }
    );
    Framebuffer fb(fbSpec);

    glm::mat4 captureViews[] =
            {
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
            };

    textureSpec.width = envTexWidth;
    textureSpec.height = envTexHeight;
    auto cubemap = Texture3D::Create(textureSpec, {});
    fb.Bind();
    envmapConverterShader->Bind();
    equirectMap->Bind();
    const auto& va = cube->GetVertexArray();
    va->Bind();
    va->GetVertexBuffer()->Bind();
    const auto projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    for (int i = 0; i < 6; i++)
    {
        envmapConverterShader->SetUniformMat4f("u_ViewProjection", projectionMatrix * captureViews[i]);
        fb.BindAttachmentToTexture3DFace(cubemap, 0, i);
        RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RendererCommand::Draw(va->GetVertexBuffer());
    }
    fb.UnBind();

    currentContext.envMap = cubemap;
}
