#pragma once

namespace editor
{
	class IPanel
	{
	public:
		IPanel() = default;
		virtual ~IPanel() = default;

		virtual void OnImGuiRender() = 0;
	};
}
