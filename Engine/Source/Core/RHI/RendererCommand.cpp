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

void RendererCommand::Configure(const RendererSettings& settings)
{
    rendererAPI->Configure(settings);
}

void RendererCommand::Draw(const std::shared_ptr<Buffer>& buffer)
{
    R_CORE_ASSERT(buffer->GetDescriptor().type == BUFFER_TYPE_VERTEX, "");
    rendererAPI->Draw(buffer);
}

void RendererCommand::DrawIndexed(const std::shared_ptr<Buffer>& vertexBuffer, const std::shared_ptr<Buffer>& indexBuffer)
{
    R_CORE_ASSERT(vertexBuffer->GetDescriptor().type == BUFFER_TYPE_VERTEX
    && indexBuffer->GetDescriptor().type == BUFFER_TYPE_INDEX
    && vertexBuffer->GetDescriptor().size > 0
    && indexBuffer->GetDescriptor().size > 0, "");
    rendererAPI->Draw(vertexBuffer, indexBuffer);
}

void RendererCommand::BeginFrame()
{
    rendererAPI->BeginFrame();
}

void RendererCommand::EndFrame()
{
    rendererAPI->EndFrame();
}
