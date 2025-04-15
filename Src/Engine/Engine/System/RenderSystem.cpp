#include <Engine/System/RenderSystem.hpp>
#include <Engine/System/TransformSystem.hpp>
#include <Engine/Service/Window/WindowService.hpp>
#include <Engine/Service/EditorService.hpp>
#include <Engine/Registration.hpp>
#include <RHI/Pipeline.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "SkyboxSystem.hpp"

namespace
{

constexpr float         C_MOUSE_SENSITIVITY = 0.002f;
constexpr float         C_EDITOR_CAMERA_SPEED = 20.0f;
constexpr glm::vec3     C_WORLD_UP = glm::vec3(0, 1, 0);

} // unnamed

RTTR_REGISTRATION
{
    using namespace engine::ecs;
    using namespace engine::registration;

    engine::registration::System<engine::CameraSystem>("engine::CameraSystem")
        .Domain(engine::Domain::UI)
        .UpdateAfter<engine::TransformSystem>();

    engine::registration::System<engine::RenderSystem>("engine::RenderSystem")
        .Domain(engine::Domain::UI)
        .UpdateAfter<engine::CameraSystem>();

    engine::registration::Component<engine::MeshComponent>(IComponent::Type::ENGINE, "engine::MeshComponent")
        .Property("mesh",
            [](const engine::MeshComponent& mesh) -> std::string
            {
                return mesh.m_mesh->SourcePath().generic_string();
            },
            [](engine::MeshComponent& obj, const std::string& meshPath)
            {
                auto& rs = engine::Instance().Service<engine::ResourceService>();
                obj.m_mesh = rs.Load<engine::MeshResource>(meshPath);
            })
        .Property("material",
            [](const engine::MeshComponent& mesh) -> std::string
            {
                return mesh.m_material->SourcePath().generic_string();
            },
            [](engine::MeshComponent& obj, const std::string& materialPath)
            {
                auto& rs = engine::Instance().Service<engine::ResourceService>();
                obj.m_material = rs.Load<engine::MaterialResource>(materialPath);
            });

    rttr::registration::enumeration<engine::CameraComponent::Type>("engine::CameraComponent::Type")
        (
            rttr::value("Editor", engine::CameraComponent::Type::EDITOR),
            rttr::value("Game", engine::CameraComponent::Type::GAME)
            );

    engine::registration::Component<engine::CameraComponent>(IComponent::Type::ENGINE, "engine::CameraComponent")
        .Property("near", &engine::CameraComponent::m_near)
        .Property("far", &engine::CameraComponent::m_far)
        .Property("aspectRatio", &engine::CameraComponent::m_aspectRatio)
        .Property("fov", &engine::CameraComponent::m_fov)
        .Property("type", &engine::CameraComponent::m_type)
        .Property("active", &engine::CameraComponent::m_active);

    engine::registration::Component<engine::DirectionalLightComponent>(IComponent::Type::ENGINE, "engine::DirectionalLightComponent");

    engine::registration::Class<engine::CameraUB>("engine::CameraUB");
    engine::registration::Class<engine::LightBufferUB>("engine::LightBufferUB");
    engine::registration::Class<engine::PBRMaterialUB>("engine::PBRMaterialUB");
}

namespace engine
{

RenderSystem::RenderSystem(ecs::World* world) : System(world)
{
}

void RenderSystem::Update(float dt)
{
    PROFILER_CPU_ZONE;

    // Currently we support only one active camera at a time
    CameraUB cameraUB{};

    for (const auto [e, c, t] : W()->View<CameraComponent, TransformComponent>())
    {
        if (!c.m_active)
        {
            continue;
        }

        cameraUB.m_position = glm::vec4(t.m_position, 1.0f);
        cameraUB.m_projView = c.m_projView;
        break;
    }

    LightBufferUB lightBufferUB{};

    for (const auto [e, l, t] : W()->View<DirectionalLightComponent, TransformComponent>())
    {
        lightBufferUB.m_directionalLight.m_position = glm::vec4(t.m_position, 1.0f);
        lightBufferUB.m_directionalLight.m_color = glm::vec4(l.m_color, 1.0f);
        lightBufferUB.m_directionalLight.m_intensity = l.m_intensity;
        lightBufferUB.m_directionalLight.m_rotation = glm::vec4(glm::eulerAngles(t.m_rotation), 1.0f);
    }

    auto& rs = Instance().Service<RenderService>();

    // Sort all meshes by pipelines
    eastl::vector_map<std::shared_ptr<rhi::Pipeline>, eastl::vector<eastl::reference_wrapper<MeshComponent>>> meshesMap;
    eastl::vector<eastl::reference_wrapper<TransformComponent>> transforms;

    bool updateSkybox = false;
    for (const auto [e, mesh, t] : W()->View<MeshComponent, TransformComponent>())
    {
        ENGINE_ASSERT(mesh.m_material);

        if (!mesh.m_mesh)
        {
            continue;
        }

        auto& pipeline = rs.Pipeline(mesh.m_material);

        meshesMap[pipeline].emplace_back(mesh);
        transforms.emplace_back(t);
        updateSkybox |= mesh.IsRecentlyCreated();
        mesh.Init();
    }

    ENGINE_ASSERT(meshesMap.size() == transforms.size());

    RPtr<EnvironmentMapResource> envmap;
    if (updateSkybox)
    {
        for (const auto [e, skybox] : W()->View<SkyboxComponent>())
        {
            envmap = skybox.m_environmentMap;
            break;
        }
    }

    eastl::unordered_set<ResPtr<MaterialResource>> materials;
    for (const auto& [pipeline, meshes] : meshesMap)
    {
        for (const auto& mesh : meshes)
        {
            if (materials.find(mesh.get().m_material) != materials.end())
            {
                continue;
            }

            mesh.get().m_material->Material()->UpdateBuffer(1, cameraUB);

            if (updateSkybox && envmap)
            {
                mesh.get().m_material->Material()->SetTexture(envmap->Raw().m_irradianceTexture, 8);
                mesh.get().m_material->Material()->SetTexture(envmap->Raw().m_prefilterTexture, 9);
                mesh.get().m_material->Material()->Sync();
            }
        }
    }

    int i = 0;
    for (const auto& [pipeline, meshes] : meshesMap)
    {
        rs.BeginPass(pipeline);

        for (const auto& mesh : meshes)
        {
            for (const auto& submesh : mesh.get().m_mesh->Mesh()->GetSubMeshList())
            {
                rs.BindMaterial(mesh.get().m_material);
                rs.PushConstant(&transforms[i].get().m_worldTransform, sizeof(glm::mat4), pipeline);

                if (submesh->IndexBuffer())
                {
                    rs.Draw(submesh->VertexBuffer(), submesh->IndexBuffer());
                }
                else
                {
                    rs.Draw(submesh->VertexBuffer(), pipeline->VertexCount(submesh->VertexBuffer()));
                }
            }
            i++;
        }

        rs.EndPass(pipeline);
    }
}

CameraSystem::CameraSystem(ecs::World* world) : System(world)
{
}

void CameraSystem::Update(float dt)
{
    PROFILER_CPU_ZONE;

    auto& windowService = Instance().Service<WindowService>();
    auto& editorService = Instance().Service<EditorService>();

    for (auto [e, c, t] : W()->View<CameraComponent, TransformComponent>())
    {
        if (!c.m_active)
        {
            continue;
        }

        if (c.m_type == CameraComponent::Type::EDITOR)
        {
            const float actualSpeed = C_EDITOR_CAMERA_SPEED * dt;

            if (windowService.KeyButtonPressed(KeyButton::KEY_W))
            {
                t.m_position += c.m_front * actualSpeed;
            }
            if (windowService.KeyButtonPressed(KeyButton::KEY_S))
            {
                t.m_position -= c.m_front * actualSpeed;
            }
            if (windowService.KeyButtonPressed(KeyButton::KEY_A))
            {
                t.m_position -= glm::normalize(glm::cross(c.m_front, c.m_up)) * actualSpeed;
            }
            if (windowService.KeyButtonPressed(KeyButton::KEY_D))
            {
                t.m_position += glm::normalize(glm::cross(c.m_front, c.m_up)) * actualSpeed;
            }

            t.Reset();
            t.Modify();
        }

        const auto prevMousePos = windowService.PrevMousePos();
        const auto mousePos = windowService.MousePos();

        // TODO: Recalculate on transform change also
        if (c.IsRecentlyCreated() || (prevMousePos != mousePos && editorService.IsViewportHovered() && windowService.MouseButtonPressed(MouseButton::BUTTON_RIGHT)))
        {
            float xOffset = mousePos.x - prevMousePos.x;
            float yOffset = prevMousePos.y - mousePos.y;
            xOffset *= C_MOUSE_SENSITIVITY;
            yOffset *= C_MOUSE_SENSITIVITY;

            // in radians
            auto rotation = glm::eulerAngles(t.m_rotation);

            rotation[0] += xOffset;
            rotation[1] += yOffset;

            rotation[1] = glm::degrees(rotation[1]);

            if (rotation[1] > 89.0f)
            {
                rotation[1] = 89.0f;
            }
            if (rotation[1] < -89.0f)
            {
                rotation[1] = -89.0f;
            }

            rotation[1] = glm::radians(rotation[1]);

            t.m_rotation = glm::quat(rotation);

            const float yaw = rotation[0];
            const float pitch = rotation[1];

            glm::vec3 rotatedFront;
            rotatedFront.x = cos(yaw) * cos(pitch);
            rotatedFront.y = sin(pitch);
            rotatedFront.z = sin(yaw) * cos(pitch);

            c.m_front = glm::normalize(rotatedFront);
            const auto right = glm::normalize(glm::cross(c.m_front, C_WORLD_UP));
            c.m_up = glm::normalize(glm::cross(right, c.m_front));

            c.Reset();
            c.Modify();

            if (c.IsRecentlyCreated())
            {
                c.Init();
            }
        }

        c.m_view = glm::lookAt(t.m_position, t.m_position + c.m_front, c.m_up);
        c.m_proj = glm::perspective(c.m_fov, c.m_aspectRatio, c.m_near, c.m_far);
        c.m_proj[1][1] *= -1;
        c.m_projView = c.m_proj * c.m_view;
    }
}

} // engine
