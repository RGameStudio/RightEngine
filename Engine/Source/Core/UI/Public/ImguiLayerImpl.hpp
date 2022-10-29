#pragma once

#include "Event.hpp"
#include "CommandBuffer.hpp"
#include "GraphicsPipeline.hpp"

namespace RightEngine
{
    class ImguiLayerImpl
    {
    public:
        ImguiLayerImpl() = default;
        virtual ~ImguiLayerImpl() = default;

        virtual void OnAttach(const std::shared_ptr<GraphicsPipeline>& pipeline) = 0;
        virtual void OnDetach() = 0;

        virtual void OnUpdate(float deltaTime) = 0;

        virtual void OnEvent(Event& event) = 0;

        virtual void Begin() = 0;
        virtual void End(const std::shared_ptr<CommandBuffer>& cmd) = 0;
    };
}