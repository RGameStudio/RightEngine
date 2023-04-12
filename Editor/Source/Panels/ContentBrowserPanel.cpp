#include "ContentBrowserPanel.hpp"
#include "Core.hpp"
#include "EditorCore.hpp"
#include "ImGuiLayer.hpp"
#include "AssetManager.hpp"
#include "TextureLoader.hpp"
#include <filesystem>
#include <imgui.h>

namespace editor
{
	ContentBrowserPanel::ContentBrowserPanel(const fs::path& currentDirectory) : currentDirectory(currentDirectory)
	{
		RightEngine::AssetManager::Get().GetLoader<RightEngine::TextureLoader>()->LoadAsync(directoryImage, "/Images/DirectoryIcon.png");
		RightEngine::AssetManager::Get().GetLoader<RightEngine::TextureLoader>()->LoadAsync(fileImage, "/Images/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (currentDirectory != fs::path(G_ASSET_DIR))
		{
			if (ImGui::Button("<-"))
			{
				currentDirectory = currentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& dirEntry : fs::directory_iterator(currentDirectory))
		{
			const auto icon = dirEntry.is_directory() ? directoryImage : fileImage;
			const auto filename = dirEntry.path().filename();
			ImGui::PushID(filename.generic_string().c_str());
			RightEngine::ImGuiLayer::ImageButton(RightEngine::AssetManager::Get().GetAsset<RightEngine::Texture>(icon), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource())
			{
				const auto enginePath = RightEngine::Path::Engine(dirEntry.path().generic_string());
				ImGui::SetDragDropPayload(C_CONTENT_BROWSER_DND_NAME, enginePath.c_str(), enginePath.size());
				ImGui::Text("%s", enginePath.c_str());
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (dirEntry.is_directory())
				{
					currentDirectory /= filename;
				}
			}
			ImGui::PopID();
			ImGui::TextWrapped(filename.generic_string().c_str());
			ImGui::NextColumn();
		}

		ImGui::Columns(1);

		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		ImGui::End();
	}

}
