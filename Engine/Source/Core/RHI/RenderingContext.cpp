#include <Windows.h>
#include <Core.h>
#include "RenderingContext.hpp"
#include <glad/glad.h>

namespace RightEngine
{
    typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext, const int *attribList);

    typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList,
                                                     UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

    wglCreateContextAttribsARB_type *wglCreateContextAttribsARB;
    wglChoosePixelFormatARB_type *wglChoosePixelFormatARB;

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B

    RenderingContext *RenderingContext::instance = nullptr;

    RenderingContext *RenderingContext::Get(HDC readDc)
    {
        if (!instance)
        {
            instance = new RenderingContext(readDc);
        }

        return instance;
    }

    RenderingContext::RenderingContext(HDC readDc)
    {
        Init(readDc);
    }

    void RenderingContext::Init(HDC readDc)
    {
        PreInit();

        // Now we can choose a pixel format the modern way, using wglChoosePixelFormatARB.
        int pixelFormatAttribs[] = {
                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
                WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                WGL_COLOR_BITS_ARB, 32,
                WGL_DEPTH_BITS_ARB, 24,
                WGL_STENCIL_BITS_ARB, 8,
                0
        };

        int pixelFormat;
        UINT numFormats;
        wglChoosePixelFormatARB(readDc, pixelFormatAttribs, 0, 1, &pixelFormat, &numFormats);

        R_CORE_ASSERT(numFormats, "Failed to set the OpenGL 3.3 pixel format.");


        PIXELFORMATDESCRIPTOR pfd;
        DescribePixelFormat(readDc, pixelFormat, sizeof(pfd), &pfd);

        R_CORE_ASSERT(SetPixelFormat(readDc, pixelFormat, &pfd), "Failed to set the OpenGL 3.3 pixel format.");

        // Specify that we want to create an OpenGL 3.3 core profile context
        int gl33Attribs[] = {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                WGL_CONTEXT_MINOR_VERSION_ARB, 3,
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                0,
        };

        HGLRC gl33Context = wglCreateContextAttribsARB(readDc, 0, gl33Attribs);

        R_CORE_ASSERT(gl33Context, "Failed to create OpenGL 3.3 context.");
        R_CORE_ASSERT(wglMakeCurrent(readDc, gl33Context), "Failed to activate OpenGL 3.3 rendering context.");
        R_CORE_ASSERT(gladLoadGL(), "Failed to load OpenGL through gladLoadGL.");

        R_CORE_INFO("Renderer vendor: {0}", glGetString(GL_VENDOR));
        R_CORE_INFO("Renderer version: {0}", glGetString(GL_VERSION));
        R_CORE_INFO("Renderer: {0}", glGetString(GL_RENDERER));
        R_CORE_INFO("Successfully initialized render context.");
    }

    void RenderingContext::PreInit()
    {
        // Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
        // We use a dummy window because you can only set the pixel format for a window once. For the
        // real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
        // that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
        // have a context.
        WNDCLASSA windowClass;
        ZeroMemory(&windowClass, sizeof(WNDCLASSA));
        windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        windowClass.lpfnWndProc = DefWindowProcA;
        windowClass.hInstance = GetModuleHandle(0);
        windowClass.lpszClassName = "Dummy_WGL_djuasiodwa";

        R_CORE_ASSERT(RegisterClassA(&windowClass), "Failed to register dummy OpenGL window.");

        HWND dummyWindow = CreateWindowExA(
                0,
                windowClass.lpszClassName,
                "Dummy OpenGL Window",
                0,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                windowClass.hInstance,
                0);


        R_CORE_ASSERT(dummyWindow, "Failed to create dummy OpenGL window.");


        HDC dummyDc = GetDC(dummyWindow);

        PIXELFORMATDESCRIPTOR pfd =
                {
                        sizeof(PIXELFORMATDESCRIPTOR),
                        1,
                        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
                        PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
                        32,                   // Colordepth of the framebuffer.
                        0, 0, 0, 0, 0, 0,
                        0,
                        0,
                        0,
                        0, 0, 0, 0,
                        24,                   // Number of bits for the depthbuffer
                        8,                    // Number of bits for the stencilbuffer
                        0,                    // Number of Aux buffers in the framebuffer.
                        PFD_MAIN_PLANE,
                        0,
                        0, 0, 0
                };

        int pixelFormat = ChoosePixelFormat(dummyDc, &pfd);

        R_CORE_ASSERT(pixelFormat, "Failed to find a suitable pixel format.");


        R_CORE_ASSERT(SetPixelFormat(dummyDc, pixelFormat, &pfd), "Failed to set the pixel format.");


        HGLRC dummyContext = wglCreateContext(dummyDc);

        R_CORE_ASSERT(dummyContext, "Failed to create a dummy OpenGL rendering context.");


        R_CORE_ASSERT(wglMakeCurrent(dummyDc, dummyContext), "Failed to activate dummy OpenGL rendering context.");


        wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type *) wglGetProcAddress("wglCreateContextAttribsARB");
        wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type *) wglGetProcAddress("wglChoosePixelFormatARB");

        wglMakeCurrent(dummyDc, nullptr);
        wglDeleteContext(dummyContext);
        ReleaseDC(dummyWindow, dummyDc);
        DestroyWindow(dummyWindow);
    }
}