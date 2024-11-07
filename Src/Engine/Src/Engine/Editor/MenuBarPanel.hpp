#pragma once

#include <Engine/Editor/Panel.hpp>

namespace engine::editor
{

	class ENGINE_API MenuBarPanel final : public Panel
	{
	public:
		MenuBarPanel() : Panel("MenuBar", true)
		{}

	private:
		virtual void DrawPanel() override;
	};

} // engine::editor