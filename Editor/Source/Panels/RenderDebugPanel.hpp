#pragma once

#include "IPanel.hpp"
#include "SceneRenderer.hpp"

namespace editor
{
	class RenderDebugPanel : public IPanel
	{
	public:
		RenderDebugPanel() = default;
		~RenderDebugPanel() = default;

		void Renderer(const std::shared_ptr<RightEngine::SceneRenderer>& renderer)
		{ m_renderer = renderer; }

		virtual void OnImGuiRender() override;

	private:
		std::shared_ptr<RightEngine::Texture> GetImageByName(std::string_view name);

		std::shared_ptr<RightEngine::SceneRenderer> m_renderer;
		int selectedImageIndex = 0;
	};
}
