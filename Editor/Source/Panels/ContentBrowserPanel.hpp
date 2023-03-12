#pragma once

#include "Core.hpp"

namespace editor
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel(const fs::path& currentDirectory = G_ASSET_DIR);

        void OnImGuiRender();

    private:
        fs::path currentDirectory;
        RightEngine::AssetHandle directoryImage;
        RightEngine::AssetHandle fileImage;
    };
}