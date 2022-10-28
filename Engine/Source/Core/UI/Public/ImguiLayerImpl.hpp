#pragma once

#include "Event.hpp"

namespace RightEngine
{
    class ImguiLayerImpl
    {
    public:
        ImguiLayerImpl() = default;
        virtual ~ImguiLayerImpl() = default;

        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;

        virtual void OnUpdate(float deltaTime) = 0;
        virtual void OnImGuiRender() = 0;

        virtual void OnEvent(Event& event) = 0;

        virtual void Begin() = 0;
        virtual void End() = 0;
    };
}