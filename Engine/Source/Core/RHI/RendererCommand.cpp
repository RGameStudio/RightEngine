#include "RendererCommand.hpp"

using namespace RightEngine;

std::shared_ptr<RendererAPI> RendererCommand::rendererAPI = nullptr;

void RendererCommand::Init(GPU_API api)
{
    rendererAPI = RendererAPI::Create(api);
}

void RendererCommand::SetClearColor(const glm::vec4& color)
{
    rendererAPI->SetClearColor(color);
}

void RendererCommand::Clear(uint32_t clearBits)
{
    rendererAPI->Clear(clearBits);
}

void RendererCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    rendererAPI->SetViewport(x, y, width, height);
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
