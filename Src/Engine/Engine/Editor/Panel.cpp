#include <Engine/Editor/Panel.hpp>
#include <imgui.h>

namespace engine::editor
{

void Panel::Draw()
{
	if (m_menuBar)
	{
		if (ImGui::BeginMainMenuBar())
		{
			m_isHovered = ImGui::IsWindowHovered();

			DrawPanel();

			ImGui::EndMainMenuBar();
		}
	}
	else
	{
		ImGui::Begin(m_name.c_str());

		m_isHovered = ImGui::IsWindowHovered();

		DrawPanel();

		ImGui::End();
	}
}

} // engine::editor