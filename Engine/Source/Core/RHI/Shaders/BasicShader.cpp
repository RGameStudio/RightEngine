#include "Shaders/BasicShader.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include <glm/ext/matrix_clip_space.hpp>

using namespace RightEngine;

RightEngine::BasicShader::BasicShader() : Shader("/Assets/Shaders/Basic/basic.vert",
                                                 "/Assets/Shaders/Basic/basic.frag")
{

}

void BasicShader::OnSetup(const std::shared_ptr<Scene>& scene)
{
    Shader::OnSetup(scene);
    const auto& renderer = Renderer::Get();
    const auto window = renderer.GetWindow();
    const auto shader = renderer.GetShader();
    const auto projectionMatrix = glm::perspective(glm::radians(45.0f),
                                                   static_cast<float>(window->GetWidth()) /
                                                   static_cast<float>(window->GetHeight()),
                                                   0.1f,
                                                   300.0f);
    SetUniformMat4f("projection", projectionMatrix);
    SetUniformMat4f("view", scene->GetCamera()->GetViewMatrix());
    SetUniform3f("cameraPos", scene->GetCamera()->GetPosition());
}

void BasicShader::OnNodeDraw(const std::shared_ptr<SceneNode>& node)
{
    Shader::OnNodeDraw(node);
    const auto& geometry = node->GetGeometry();
    if (geometry)
    {
        const auto material = geometry->GetMaterial();
        const auto& materialData = material->GetMaterialData();
        const auto baseTexture = material->GetBaseTexture();
        if (baseTexture)
        {
            material->GetBaseTexture()->Bind();
            SetUniform1i("baseTexture", 0);
            SetUniform1i("hasBaseTexture", true);
        }
        else
        {
            SetUniform1i("hasBaseTexture", false);
        }

        SetUniform3f("material.diffuse", materialData.diffuse);
        SetUniform3f("material.specular", materialData.specular);
        SetUniform3f("material.ambient", materialData.ambient);
        SetUniform1f("material.shininess", materialData.shininess);

        SetUniformMat4f("model", node->GetWorldModelMatrix());
    }
}

void BasicShader::OnLightSave(LightInfo lightInfo)
{
    Shader::OnLightSave(lightInfo);
    SetUniform1i("hasAmbient", lightInfo.hasAmbient);
    SetUniform3f("ambientColor", lightInfo.ambientColor);

    SetUniform1i("pointLightAmount", lightInfo.pointLightAmount);
    for (int i = 0; i < lightInfo.pointLightAmount; i++)
    {
        SetUniform3f("pointLightPos[" + std::to_string(i) + "]", lightInfo.pointLightPosition[i]);
        SetUniform3f("pointLightColor[" + std::to_string(i) + "]", lightInfo.pointLightColor[i]);
    }
}


