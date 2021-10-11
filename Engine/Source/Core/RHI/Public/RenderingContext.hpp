#pragma once

#include <Windows.h>

namespace RightEngine
{
    class RenderingContext
    {
    public:
        static RenderingContext* Get(HDC readDc);
    private:
        RenderingContext(HDC readDc);
        virtual void Init(HDC readDc);
        virtual void PreInit();

        static RenderingContext* instance;
    };
}