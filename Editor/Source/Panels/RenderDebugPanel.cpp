#include "RenderDebugPanel.hpp"
#include "ImGuiLayer.hpp"
#include <imgui.h>

namespace 
{
	constexpr const char* C_DIR_LIGHT_DEPTH_IMAGE = "Directional Light Depth";
}

namespace editor
{
	void RenderDebugPanel::OnImGuiRender()
	{
		R_CORE_ASSERT(m_renderer, "");
		ImGui::Begin("Render debug");

		std::array<std::string, 2> images = { "None", C_DIR_LIGHT_DEPTH_IMAGE};

		if (ImGui::BeginCombo("Image", images.front().c_str()))
		{
			for (int i = 0; i < images.size(); i++)
			{
				bool isSelected = selectedImageIndex == i;
				if (ImGui::Selectable(images.at(i).c_str(), isSelected))
				{
					selectedImageIndex = i;
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (selectedImageIndex == 0)
		{
			ImGui::End();
			return;
		}

		const auto texture = GetImageByName(images.at(selectedImageIndex));
		RightEngine::ImGuiLayer::Image(texture, ImVec2(256, 256));

		ImGui::End();
	}

	std::shared_ptr<RightEngine::Texture> RenderDebugPanel::GetImageByName(std::string_view name)
	{
		if (name == C_DIR_LIGHT_DEPTH_IMAGE)
		{
			return m_renderer->GetPass(RightEngine::PassType::SHADOW)->GetRenderPassDescriptor().depthStencilAttachment.texture;
		}
	}
}
