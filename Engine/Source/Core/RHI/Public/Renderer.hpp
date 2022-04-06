#pragma once

#include "Window.hpp"
#include "Components.hpp"
#include "Config.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Scene.hpp"
#include "Types.hpp"
#include <glm/matrix.hpp>

namespace RightEngine
{
    struct SceneData
    {
        glm::mat4 viewProjectionMatrix{ 1.0f };
    };

    struct RendererSettings
    {
        bool hasDepthTest{ true };
    };

    class Renderer
    {
    public:
        void SubmitMesh(const std::shared_ptr<Shader>& shader,
                            const Mesh& mesh,
                            const glm::mat4& transform);

        void BeginScene(const std::shared_ptr<Scene>& scene);
        void EndScene();

        void Configure();

        RendererSettings& GetSettings();

    private:
        SceneData sceneData;
        RendererSettings settings;
    };
}