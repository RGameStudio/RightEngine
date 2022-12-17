#pragma once

#include "Scene.hpp"
#include "MeshLoader.hpp"
#include "UniformBufferSet.hpp"
#include "Renderer.hpp"

namespace RightEngine
{
    struct SceneRendererSettings
    {
        float gamma{ 1.7f };
    };

    enum class PassType
    {
        PBR,
        SKYBOX,
        POSTPROCESS,
        UI,
        PRESENT
    };

    struct LightData
    {
        glm::vec4 color;
        glm::vec4 position;
        glm::vec4 rotation;
        float intensity;
        int type;
    };

    class SceneRenderer
    {
    public:
        void Init();

        const std::shared_ptr<Scene>& GetScene() const
        { return scene; }

        void SetScene(const std::shared_ptr<Scene>& aScene)
        { scene = aScene; }

        void SubmitMeshNode(const std::shared_ptr<MeshNode>& meshNode, const std::shared_ptr<Material>& material, const glm::mat4& transform);
        void SubmitMesh(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const glm::mat4& transform);

        void BeginScene(const std::shared_ptr<Camera>& camera,
                        const std::shared_ptr<EnvironmentContext>& environment,
                        const std::vector<LightData>& lights,
                        const SceneRendererSettings& rendererSettings = {});
        void EndScene();

        void Resize(int x, int y);

        void SetUIPassCallback(std::function<void(const std::shared_ptr<CommandBuffer>&)>&& callback)
        { uiPassCallback = callback; }

        const std::shared_ptr<GraphicsPipeline>& GetPass(PassType type) const;
        const std::shared_ptr<Texture>& GetFinalImage() const;

    private:
        void CreateShaders();
        void CreatePasses();
        void CreateOffscreenPasses();
        void CreateOnscreenPasses();
        void CreateBuffers();

        // Passes
        void PBRPass();
        void SkyboxPass();
        void PostprocessPass();
        void UIPass();
        void Present();
        void Clear();

        // Renderer data
        std::shared_ptr<Scene> scene;
        std::shared_ptr<Sampler> defaultSampler;
        bool isInitialized{ false };
        glm::ivec2 viewport{ 1280, 720 };
        glm::ivec2 windowSize{ 1280, 720 };
        std::shared_ptr<UniformBufferSet> uniformBufferSet;
        Renderer renderer;
        std::shared_ptr<Buffer> skyboxVertexBuffer;
        std::shared_ptr<Buffer> fullscreenQuadVertexBuffer;
        std::function<void(const std::shared_ptr<CommandBuffer>&)> uiPassCallback;

        //Pipelines
        std::shared_ptr<GraphicsPipeline> pbrPipeline;
        std::shared_ptr<GraphicsPipeline> skyboxPipeline;
        std::shared_ptr<GraphicsPipeline> postprocessPipeline;
        std::shared_ptr<GraphicsPipeline> uiPipeline;
        std::shared_ptr<GraphicsPipeline> presentPipeline;

        // TODO: Move shaders to ShaderLibrary
        std::shared_ptr<Shader> pbrShader;
        std::shared_ptr<Shader> skyboxShader;
        std::shared_ptr<Shader> postprocessShader;

        struct DrawCommand
        {
            std::shared_ptr<Mesh> mesh;
            std::shared_ptr<Material> material;
            glm::mat4 transform;
        };

        struct UBCameraData
        {
            glm::mat4 viewProjection;
            glm::vec4 position;
        } cameraDataUB;

        struct UBLightData
        {
            int lightsAmount;
            LightData light[30];
        } lightDataUB;

        struct UBTransformData
        {
            glm::mat4 transform;
        } transformDataUB;

        // Per frame data
        std::vector<DrawCommand> drawList;
        EnvironmentContext sceneEnvironment;
    };
}