#pragma once

#include "Window.hpp"
#include "Geometry.hpp"
#include "Config.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
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
        void SubmitGeometry(const std::shared_ptr<Shader>& shader,
                            const std::shared_ptr<Geometry>& geometry,
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