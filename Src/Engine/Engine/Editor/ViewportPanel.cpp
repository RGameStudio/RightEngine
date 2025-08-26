#include <Engine/Editor/ViewportPanel.hpp>
#include <Engine/Service/Imgui/ImguiService.hpp>
#include <Engine/Service/Render/RenderService.hpp>
#include <RHI/Pipeline.hpp>

#include "Engine/Service/EditorService.hpp"
#include "Engine/Service/Window/WindowService.hpp"
#include "Engine/Service/World/WorldService.hpp"
#include "Engine/System/RenderSystem.hpp"
#include "Engine/System/TransformSystem.hpp"
#include <glm/gtc/type_ptr.hpp>


namespace engine::editor
{

void ViewportPanel::DrawPanel()
{
    auto& rs = Instance().Service<RenderService>();
    auto& is = Instance().Service<ImguiService>();

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();

    if (!core::math::almostEqual(m_viewportSize.x, viewportSize.x) || !core::math::almostEqual(m_viewportSize.y, viewportSize.y))
    {
        m_viewportSize = { viewportSize.x, viewportSize.y };
        rs.OnResize({ static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y) });
    }

    is.Image(rs.Pipeline(rs.DefaultMaterial())->Descriptor().m_pass->Descriptor().m_colorAttachments[0].m_texture, m_viewportSize);
    DrawGizmos();
}

void ViewportPanel::DrawGizmos()
{
    auto& editorService = Instance().Service<EditorService>();
    auto& world = Instance().Service<WorldService>().CurrentWorld();
    auto& em = world->GetEntityManager();
    auto& windowService = Instance().Service<WindowService>();

    const auto selectedEntity = editorService.SelectedEntity();

    if (selectedEntity == C_INVALID_ENTITY)
    {
        return;
    }

    if (ImGui::IsWindowHovered())
    {
        if (windowService.KeyButtonPressed(KeyButton::KEY_T))
        {
            m_gizmoType = ImGuizmo::TRANSLATE;
        }
        else if (windowService.KeyButtonPressed(KeyButton::KEY_R))
        {
            m_gizmoType = ImGuizmo::ROTATE;
        }
        else if (windowService.KeyButtonPressed(KeyButton::KEY_Y))
        {
            m_gizmoType = ImGuizmo::SCALE;
        }
    }

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

    CameraComponent camera;
    for (auto [e, c] : world->View<CameraComponent>())
    {
        camera = c;
        break;
    }

    glm::mat4 cameraView = camera.m_view;
    glm::mat4 cameraProjection = camera.m_proj;
    cameraProjection[1][1] *= -1;

    auto& entityTransformComponent = em->GetComponent<TransformComponent>(selectedEntity);
    glm::mat4 entityTransform = entityTransformComponent.m_worldTransform;

    ImGuizmo::Manipulate(glm::value_ptr(cameraView),
        glm::value_ptr(cameraProjection),
        m_gizmoType,
        ImGuizmo::LOCAL,
        glm::value_ptr(entityTransform));

    if (ImGuizmo::IsUsing())
    {
        glm::vec3 position, rotation, scale;
        [[maybe_unused]] auto decomposeResult = core::math::decomposeTransform(entityTransform, position, rotation, scale);
        ENGINE_ASSERT_WITH_MESSAGE(decomposeResult, "Decompose failed");

        auto deltaRotation = rotation - glm::eulerAngles(entityTransformComponent.m_rotation);
        entityTransformComponent.m_position = position;
        entityTransformComponent.m_rotation = glm::quat(rotation + deltaRotation);
        entityTransformComponent.m_scale = scale;
    }
}
} // engine::editor
