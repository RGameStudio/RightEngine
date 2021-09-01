#include "RenderingContext.hpp"

namespace RightEngine
{
    RenderingContext *RenderingContext::instance = nullptr;

    RenderingContext *RenderingContext::Get()
    {
        if (!instance)
        {
            instance = new RenderingContext();
        }

        return instance;
    }

    RenderingContext::RenderingContext()
    {

    }
}