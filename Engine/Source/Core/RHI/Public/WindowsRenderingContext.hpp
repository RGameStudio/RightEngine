#ifdef WIN32
#pragma once

#include <Windows.h>

namespace RightEngine
{
    enum class VSyncState
    {
        ON,
        OFF
    };

    class WindowsRenderingContext
    {
    public:
        static WindowsRenderingContext *Get(HDC readDc);

        void SetVSync(VSyncState state);

    private:
        WindowsRenderingContext(HDC readDc);

        virtual void Init(HDC readDc);

        virtual void PreInit();

        static WindowsRenderingContext *instance;
    };
}
#endif