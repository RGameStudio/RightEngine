#pragma once

namespace RightEngine
{
    class RenderingContext
    {
    public:
        static RenderingContext* Get();
    private:
        RenderingContext();

        static RenderingContext* instance;
    };
}