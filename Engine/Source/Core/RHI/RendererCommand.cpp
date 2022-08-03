#include "RendererCommand.hpp"

using namespace RightEngine;

std::shared_ptr<RendererAPI> RendererCommand::rendererAPI = nullptr;

void RendererCommand::Init(GPU_API api)
{
    rendererAPI = RendererAPI::Create(api);
    rendererAPI->Init();
}

void RendererCommand::SetClearColor(const glm::vec4& color)
{
    rendererAPI->SetClearColor(color);
}

void RendererCommand::Clear(uint32_t clearBits)
{
    // TODO: Use API agnostic bits
    rendererAPI->Clear(clearBits);
}

void RendererCommand::SetViewport(const Viewport& viewport)
{
    rendererAPI->SetViewport(viewport);
}

Viewport RendererCommand::GetViewport()
{
    return rendererAPI->GetViewport();
}

void RendererCommand::DrawIndexed(const std::shared_ptr<IndexBuffer>& ib)
{
    rendererAPI->DrawIndexed(ib);
}

void RendererCommand::Draw(const std::shared_ptr<VertexBuffer>& vb)
{
    rendererAPI->Draw(vb);
}

void RendererCommand::Configure(const RendererSettings& settings)
{
    rendererAPI->Configure(settings);
}

void RendererCommand::BeginFrame()
{
    rendererAPI->BeginFrame();
}

void RendererCommand::EndFrame()
{
    rendererAPI->EndFrame();
}
