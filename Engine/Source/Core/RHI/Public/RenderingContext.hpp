#pragma once

#include <Windows.h>

namespace RightEngine
{
    enum class VSyncState
    {
        ON,
        OFF
    };

    class RenderingContext
    {
    public:
        static RenderingContext *Get(HDC readDc);

        void SetVSync(VSyncState state);

    private:
        RenderingContext(HDC readDc);

        virtual void Init(HDC readDc);

        virtual void PreInit();

        static RenderingContext *instance;
    };
}