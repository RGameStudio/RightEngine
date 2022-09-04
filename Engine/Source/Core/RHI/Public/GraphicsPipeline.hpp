#pragma once

#include "GraphicsPipelineDescriptor.hpp"

namespace RightEngine
{
    class GraphicsPipeline
    {
    public:
        const GraphicsPipelineDescriptor& GetPipelineDescriptor() const
        { return pipelineDescriptor; }

        const RenderPassDescriptor& GetRenderPassDescriptor() const
        { return renderPassDescriptor; }

        const std::shared_ptr<Texture>& GetDepthStencilAttachment() const
        { return depthStencilAttachment; }

        virtual void CreateDepthStencilAttachment(int width, int height) = 0;

    protected:
        GraphicsPipeline(const GraphicsPipelineDescriptor& descriptor,
                         const RenderPassDescriptor& renderPassDescriptor) : pipelineDescriptor(descriptor),
                                                                             renderPassDescriptor(renderPassDescriptor)
        {}

        GraphicsPipelineDescriptor pipelineDescriptor;
        RenderPassDescriptor renderPassDescriptor;
        std::shared_ptr<Texture> depthStencilAttachment;
    };
}